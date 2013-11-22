<?php
/**
 * a php library for the vircurex API
 * 
 * please not that there is no waranty on using this package. please do test it before production use!
 * 
 * @license general gnu GPLv3
 * @author ulkas
 */

class CData {
	/**#@+
	 * user specific config data. change this
	 */
	protected $account	=	"vircurex";
	/**
	 * your secrets. check the value of CData::$APIprivate for more info
	 */
	protected $secrets	=	array(
			0=>'a',
			1=>'a',			//this secret is the same as the first one (0).
			2=>'b',
			3=>'c',
			4=>'d',
			5=>'e',
			6=>'f',
			7=>'g',
			8=>'h',
			9=>'i');
	/**#@-*/
	/**#@+
	 * general vircurex info
	 */
	protected $domain	=	"vircurex.com";
	protected $apitype	=	"json";
	protected $userAgent=	"ulkas's vircurex api bot";
	protected $http		=	"https://";
	protected $https	=	"https://";
	protected $apidir	=	'/api';
	protected static $currencies=	array('ANC','BTC','DGC','DVC','FRC','FTC','I0C','IXC','LTC','NMC','NVC','PPC','TRC','WDC','XPM');
	/**
	 * public api names
	 * @var array
	 */
	protected static $APIpublic=	array(
			0=>'get_lowest_ask',
			1=>'get_highest_bid',
			2=>'get_last_trade',
			3=>'get_volume',
			4=>'get_info_for_currency',
			5=>'get_info_for_1_currency',
			6=>'orderbook',
			7=>'orderbook_alt',
			8=>'trades',
			9=>'get_currency_info'
	);
	/**
	 * private API names
	 * @var array
	 */
	protected static $APIprivate=	array(
			0=>'get_balance',
			1=>'get_balances',
			2=>'create_order',
			3=>'release_order',
			4=>'delete_order',
			5=>'read_order',
			6=>'read_orders',
			7=>'read_orderexecutions',
			8=>'create_coupon',
			9=>'redeem_coupon'
	);
	/**
	 * output variable names which are included in the output token
	 * @var array
	 */
	protected static $APIoutputs=	array(
			0=>array('balance'),
			1=>array(),
			2=>array('orderid'),
			3=>array('orderid'),
			4=>array('orderid'),
			5=>array('orderid'),
			6=>array(),
			7=>array(),
			8=>array('coupon','amount','currency'),
			9=>array('coupon','amount','currency'),
	);
	/**
	 * this can be of any value
	 * 
	 * but must be unique. an id cannot be used 2x within 10minutes
	 * @var string
	 */
	protected $id;
	protected $token;
	/**
	 * @var string
	 */
	protected $timestamp;
	protected $dataPublic	=	array();
	protected $dataPrivate	=	array();
	/**
	 * the name of the current function (api method call)
	 * @var string
	 */
	protected $method;
	/**
	 * index in the above arrays. shall be the same value of the method row
	 * @var integer
	 */
	protected $index;
	/**
	 * just garbage
	 * @var integer
	 */
	protected static $foo	=	1;
	/**#@-*/

	public function __construct($apitype="json"){
		$time=time();
		$this->apitype=$apitype;
		$this->id=substr(md5($time."ebenci".self::$foo++),5,10);		//id is any unique string
		$this->timestamp=gmdate('Y-m-d\TG:i:s',$time);
		//general info about api - name + url
		foreach (self::$APIpublic as $key=>$name){
			$this->dataPublic[$name]=$this->http.$this->domain.$this->apidir.'/'.$name.'.'.$this->apitype;
		}
		//trade info about api - name + url
		foreach (self::$APIprivate as $key=>$name){
			$this->dataPrivate[$name]=$this->https.$this->domain.$this->apidir.'/'.$name.'.'.$this->apitype;
		}
	}
	/**
	 * handles all method calls which are not strictly defined as a function
	 * 
	 * @example	new self->SOMEMETHODNAME() points to this function
	 * @param string$func
	 * @param array $params
	 */
	public function __call($func, $params=array()){
		//current api method name
		$this->method=$func;
		//the method name is a public one
		if(in_array($this->method, self::$APIpublic)){
			//some public api methods have 1 or less params
			if(!isset($params[0]))$params[0]=false;
			if(!isset($params[1]))$params[1]=false;
			return $this->callPublic($params[0],$params[1]);
		}
		//private method name
		if(in_array($this->method, self::$APIprivate)){
			//could be done better
			$tmp=array_flip(self::$APIprivate);
			$this->index=$tmp[$this->method];
			unset($tmp);
			//some private methods have no input params
			if(isset($params[0])) return $this->callPrivate($params[0]);
			return $this->callPrivate();
		}
	}
	/**
	 * public curl api call
	 * @param string $cur1
	 * @param string $cur2
	 */
	protected function callPublic($cur1,$cur2,$params=""){
		$params=trim($params);
		$cur1=strtoupper($cur1);
		$cur2=strtoupper($cur2);
		//unknown currency
		if(!in_array($cur1, self::$currencies))return false;
		if(!in_array($cur2, self::$currencies))return false;
		$url=$this->dataPublic[$this->method]."?base=$cur1&alt=$cur2".$params;
		$data=$this->curl($url);
		$data=json_decode($data);
		return $data;
	}
	protected function callPrivate($params=array()){
		if(!is_array($params))return false;
		$data="";
		foreach ($params as $param=>$value) {
			$data.='&'.$param.'='.$value;
		}
		$data='account='.$this->account.'&id='.$this->id.'&token='.$this->token($params).'&timestamp='.$this->timestamp.$data;
		$url=$this->dataPrivate[$this->method]."?".$data;
		$data=$this->curl($url);
		$data=json_decode($data);
		switch ($data->status) {
			/**
			 * @todo: vircurex lame
			 */
			case 0: /* $this->tokenCheck($data); */ break;			//TODO:			-----------!!!!!!!!! there seems to be a problem on the server side with the returnig token string
			default: die($data->status . ":" .$data->statustxt);
		}
		return $data;
	}
	/**
	 * generate token
	 * @param array $params
	 */
	protected function token($params=array()){
		//delimiter
		$d=';';
		$data="";
		if(count($params)>0)$data=$d.implode($d, $params);
		$hash=hash('sha256',$this->secrets[$this->index].$d.$this->account.$d.$this->timestamp.$d.$this->id.$d.$this->method.$data);
		return $hash;
	}
	/**
	 * compare server token
	 * @param json_decode $object
	 */
	protected function tokenCheck($object){
		//delimiter		
		$d=';';
		$data="";
		//build the tokenized values from the server response
		foreach (self::$APIoutputs[$this->index] as $param){
			$data.=$d.$object->{$param};
		}
		$mine=hash('sha256',$this->secrets[$this->index].$d.$this->account.$d.$this->timestamp.$d.$this->method.$data);
		if ($mine!=$object->token){
			die('answer token missmatch');
		}
		return $mine==$token;
	}

	/**
	 * the only public method with more than 2 params. defined separately
	 * 
	 * Returns all executed trades of the past 7 days. If the parameter "since" is provided, then only trades with an order ID greater than "since" will be returned.
	 * @param string $cur1
	 * @param string $cur2
	 * @param integer $since
	 */
	public function trades($cur1, $cur2, $since=""){
		if($since)$since=intval($since);
		return $this->callPublic(__FUNCTION__, $cur1, $cur2,"&since=$since");
	}
	/**
	 * curl. handles error response
	 * @param string $target_url
	 */
	protected function curl($target_url){
		$target_url=trim($target_url);
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_USERAGENT, $this->userAgent);
		curl_setopt($ch, CURLOPT_URL,$target_url);
		//curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
		curl_setopt($ch, CURLOPT_AUTOREFERER, true);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER,true);
		curl_setopt($ch, CURLOPT_TIMEOUT, 5);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 3);
		curl_setopt($ch, CURLOPT_MAXREDIRS, 0);
		curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 1);
		$html= curl_exec($ch);
		if (!$html) {
			$html= "cURL error number:" .curl_errno($ch);
			$html .= " cURL error:" . curl_error($ch);
		}
		curl_close($ch);
		switch($this->apitype){
			case 'json':	if($html[0]!='{') die($html);break;
			default:		if($html[0]!='<') die($html);break;
		}
		//ubber. no need to comment how this library handles xml
		if($this->apitype!="json"){
			$html = simplexml_load_string($html);
			$html = json_encode($html);
		}
		return $html;
	}
}