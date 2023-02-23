'use strict';
//import { h, html, render, useEffect, useState } from './preact.min.js';

var input1=1;
var input2=2;

var E = function(id) { return document.getElementById(id); };
var btn1 = E('btn1'), btn2 = E('btn2'), msglog = E('log');
var btn3 = E('btn3');
var enable = function(en) { btn1.disabled = btn2.disabled = !en; };
var log = text => msglog.innerHTML += text + '<br/>\n';

enable(true);
//btn3.disabled = true;

//btn2.innerHTML="Call SUM(${input1}, ${input2})"
//btn2.innerHTML="Call SUM(" + input1 +", "+ input2 + ")";
zsp_fun();

btn1.onclick = ev => fetch('/api/f1')
    .then(r => r.json())
    .then(r => {
        console.log(r);
        log('GET /api/f1: ' + JSON.stringify(r) + ' -> ' + JSON.stringify(r.result))
    })
    .catch(err => console.log(err));

const getsum = (a, b) =>
    fetch('/api/sum', {method: 'POST', body:JSON.stringify([a, b])})
        .then(r => r.json())
        .catch(err => console.log(err));
var input_test = 5;
//btn2.onclick = ev => getsum(input1, input2)
btn2.onclick = ev => getsum(input_test, input2)
    .then(r => {
        console.log(r);
        //log('POST [' + input1 + ', ' + input2 + '] to /api/sum: ' + JSON.stringify(r) + ' -> ' + JSON.stringify(r.result))
        log('POST /api/sum: ' + JSON.stringify(r) + ' -> ' + JSON.stringify(r.result))
    })
    .catch(err => console.log(err));

function zsp_fun(){
    input1 = E('text1').value;
    input2 = E('text2').value;
    //let x=document.getElementById('btn2');
    btn2.innerHTML="Call SUM(" + input1 +", "+ input2 + ")";
}

async function test_interface(){
    //window.location.href="http://127.0.0.1:8000/web_root";

    //fetch('/api/sum', {method: 'POST', body:JSON.stringify([1, 2])});
    //try {
    let response = await fetch('/api/fn3', {method: 'POST', body:JSON.stringify([1, 2])});
    let json = await response.json();
    if (response.status == 200) {
        log('get location ' + JSON.stringify(json));

        // test
        /*
        E('info1').innerText="Bit rate: ";
        E('value1').innerHTML=json.CONFIG_BIT_RATE;
        E('select1').selectedIndex=json.CONFIG_BIT_RATE;

        E('info2').innerHTML="CONFIG_PICTURE_SIZE";
        E('value2').innerHTML=json.CONFIG_PICTURE_SIZE;

        E('info3').innerHTML="CONFIG_BAUD_RATE";
        E('value3').innerHTML=json.CONFIG_BAUD_RATE;
        */

        E('select_bit_rate').selectedIndex=json.CONFIG_BIT_RATE;
        E('select_resolution').selectedIndex=json.CONFIG_RESOLUTION;
        E('select_horizontal_image').selectedIndex=json.CONFIG_HORIZONTAL_IMAGE;
        E('select_vertical_image').selectedIndex=json.CONFIG_VERTICAL_IMAGE;
        E('select_color_mode').selectedIndex=json.CONFIG_COLOR_MODE;
        E('select_white_light').selectedIndex=json.CONFIG_WHITE_LIGHT;
        E('select_record').selectedIndex=json.CONFIG_RECORD;
        E('select_ntp').selectedIndex=json.CONFIG_NTP;
        //window.location.href=json.url;
    }
    return;
    //return await response.json();
    //} catch (error) {
    //  console.log('Request Failed', error);
    //}

}
btn3.onclick = ev => test_interface();

/*
function config_submit(){
    alert('set failed');
}
*/
function ConfigJSON(){
    return JSON.stringify({"CONFIG_BIT_RATE":E('select_bit_rate').selectedIndex,
                        "CONFIG_RESOLUTION":E('select_resolution').selectedIndex,
                        "CONFIG_HORIZONTAL_IMAGE":E('select_horizontal_image').selectedIndex,
                        "CONFIG_VERTICAL_IMAGE":E('select_vertical_image').selectedIndex,
                        "CONFIG_COLOR_MODE":E('select_color_mode').selectedIndex,
                        "CONFIG_WHITE_LIGHT":E('select_white_light').selectedIndex,
                        "CONFIG_RECORD":E('select_record').selectedIndex,
                        "CONFIG_NTP":E('select_ntp').selectedIndex});
}
async function ConfigSubmit(){
    try {
        let response = await fetch('/api/submit', {method: 'POST', body:ConfigJSON()});
        let text = await response.text();
        alert(text);
    } catch (error) {
        console.log('Request Failed', error);
        alert('set failed');
    }
}
E('btn_submit').onclick = ev => ConfigSubmit();

//window.onload = () => render(h(App), document.body);

window.onload = function(){
    test_interface();
}


