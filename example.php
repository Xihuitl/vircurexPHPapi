<?php
include_once 'inc/data.h';

$data=new CData();
//example public
var_dump($data->get_info_for_1_currency('BTC','PPC'));
//example private
var_dump($data->create_order(array('ordertype'=>'SELL','amount'=>'0.0001','currency1'=>'PPC','unitprice'=>'200.2','currency2'=>'BTC')));
?>
