'use strict';
import { h, html, render, useEffect, useState } from './preact.min.js';

//var main_page = document.body;

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

var time1;
async function GetDeviceInfo(){
    /*
    fetch('/api/device_info')
        .then(r => r.text())
        .then(r => {
            console.log(r);
            log('GET /api/device_info: ' + r);
        })
        .catch(err => {window.clearInterval(time1);console.log(err)});
        */
    try {
        let response = await fetch('/api/device_info');
        if (response.status == 200) {
            let json = await response.json();
            //console.log(json);
            log('GET /api/device_info: ' + JSON.stringify(json));
            E('s_sim').innerHTML=json.STATUS_SIM;
            E('s_imei').innerHTML=json.STATUS_IMEI;
            E('s_imsi').innerHTML=json.STATUS_IMSI;
            E('s_reg').innerHTML=json.STATUS_REG;
            E('s_channel').innerHTML=json.STATUS_CHANNEL;
            E('s_rsrp').innerHTML=json.STATUS_RSRP;
            E('s_sinr').innerHTML=json.STATUS_SINR;
            E('s_ip').innerHTML=json.STATUS_IP;
            E('s_mac').innerHTML=json.STATUS_MAC;
            E('s_gateway').innerHTML=json.STATUS_GATEWAY;
            E('s_dns').innerHTML=json.STATUS_DNS;
        }
        else {
            //window.clearInterval(time1);
            location.reload();
        }
    } catch (error) {
        window.clearInterval(time1);
        console.log(error);
    }
}

async function GetConfigJSON(){
    //fetch('/api/sum', {method: 'POST', body:JSON.stringify([1, 2])});

    //try {
    let response = await fetch('/api/settings', {method: 'POST', body:JSON.stringify([1, 2])});
    let json = await response.json();
    if (response.status == 200) {
        log('get location ' + JSON.stringify(json));

        E('select_bit_rate').selectedIndex=json.CONFIG_RTSP_BITRATE;
        E('select_resolution').selectedIndex=json.CONFIG_RESOLUTION;
        E('select_frame_rate').selectedIndex=json.CONFIG_FRAME_RATE;
        E('select_horizontal_image').selectedIndex=json.CONFIG_SENSOR_HRZ_MIRROR;
        E('select_vertical_image').selectedIndex=json.CONFIG_SENSOR_TILT_MIRROR;
        E('select_color_mode').selectedIndex=json.CONFIG_DAYNIGHT;
        E('select_white_light').selectedIndex=json.CONFIG_WHITE_LED;
        E('select_record').selectedIndex=json.CONFIG_RECORD_ENABLE;
        E('select_ntp').selectedIndex=json.CONFIG_SYNCTIME_ENABLE;
        E('select_net').selectedIndex=json.CONFIG_NET;
        E('rtsp_port').value=json.CONFIG_RTSP_PORT;
        E('ping_ip').value=json.CONFIG_PING_IP;
        E('ip_address').value=json.CONFIG_IP_ADDRESS;
        E('ntp_server').value=json.CONFIG_NTPSERVER1;
        E('select_time_zone').selectedIndex=json.CONFIG_TIMEZONE;
    }
    return;
    //return await response.json();
    //} catch (error) {
    //  console.log('Request Failed', error);
    //}
}

async function test_interface(){
    window.clearInterval(time1);
    GetDeviceInfo();
    time1 = window.setInterval(GetDeviceInfo, 10000 * 3);

    //window.location.href="http://127.0.0.1:8000/web_root";

    GetConfigJSON();
}
btn3.onclick = ev => test_interface();

/*
function config_submit(){
    alert('set failed');
}
*/
function ConfigJSON(){
    return JSON.stringify({
                          "CONFIG_RTSP_BITRATE":E('select_bit_rate').selectedIndex,
                          "CONFIG_RESOLUTION":E('select_resolution').selectedIndex,
                          "CONFIG_FRAME_RATE":E('select_frame_rate').selectedIndex,
                          "CONFIG_SENSOR_HRZ_MIRROR":E('select_horizontal_image').selectedIndex,
                          "CONFIG_SENSOR_TILT_MIRROR":E('select_vertical_image').selectedIndex,
                          "CONFIG_DAYNIGHT":E('select_color_mode').selectedIndex,
                          "CONFIG_WHITE_LED":E('select_white_light').selectedIndex,
                          "CONFIG_RECORD_ENABLE":E('select_record').selectedIndex,
                          "CONFIG_SYNCTIME_ENABLE":E('select_ntp').selectedIndex,
                          "CONFIG_NET":E('select_net').selectedIndex,
                          "CONFIG_RTSP_PORT":E('rtsp_port').value,
                          "CONFIG_PING_IP":E('ping_ip').value,
                          "CONFIG_IP_ADDRESS":E('ip_address').value,
                          "CONFIG_NTPSERVER1":E('ntp_server').value,
                          "CONFIG_TIMEZONE":E('select_time_zone').selectedIndex,
    });
}
async function ConfigSubmit(id){
    //E('btn1_save').disabled = true;
    //E('btn2_save').disabled = true;
    E(id).disabled = true;
    try {
        let response = await fetch('/api/submit', {method: 'POST', body:ConfigJSON()});
        let text = await response.text();
        alert(text);
        if (response.status != 200) {
            location.reload();
        }
    } catch (error) {
        console.log('Request Failed', error);
        alert('set failed');
    }
    //E('btn1_save').disabled = false;
    //E('btn2_save').disabled = false;
    E(id).disabled = false;
}

E('btn1_save').onclick = ev => ConfigSubmit('btn1_save');
E('btn2_save').onclick = ev => ConfigSubmit('btn2_save');
E('btn3_save').onclick = ev => ConfigSubmit('btn3_save');
E('btn4_save').onclick = ev => ConfigSubmit('btn4_save');

async function UpLoad(){
    E('btn_upload').disabled = true;
    const file = E('file_upload').files[0];
    var data;
    var reader = new FileReader();
    reader.onload = function() {
        data = reader.result.split(",")[1];
        //console.log(data);
        fetch('/api/upgrade', { method: 'POST', body: data }
        ).then(
            response => response.text() // if the response is a JSON object
        ).then(
            text => {
                console.log(text); // Handle the success response object
                alert(text)
            }
        ).catch(
            error => {
                console.log(error); // Handle the error response object
                alert('send failed')
            }
        );
        E('btn_upload').disabled = false;
    }
    reader.readAsDataURL(file);
}

E('file_upload').addEventListener('change', (event) => {
    const files = event.target.files;
    console.log('files object', files);
    console.log(files.length);
    if (files.length > 0 && files[0].size < 10 * 1024 * 1024) {
        E('btn_upload').disabled = false;
    }
    else {
        E('btn_upload').disabled = true;
    }
    if (files.length > 0 && files[0].size > 10 * 1024 * 1024) {
        alert('上传文件的大小需要小于10MB');
    }
});
E('btn_upload').onclick = ev => UpLoad();

var f; // selected file

// When user clicks on a button, trigger file selection dialog
document.getElementById('el2').onclick = function(ev) {
    document.getElementById('el1').click();
};

document.getElementById('el5').onclick = function(ev) {
    var r = new FileReader();
    r.readAsArrayBuffer(f);
    r.onload = function() {
        E('el5').disabled = true;
        ev.target.value = '';
        document.getElementById('el3').innerText = 'Uploading...';
        fetch('/api/upload?name=' + encodeURIComponent(f.name), {
            method: 'POST',
            body: r.result,}
        ).then(response => response.text()
        ).then(text =>{
            document.getElementById('el3').innerText = 'Uploaded ' + r.result.byteLength + ' bytes  ';
            alert(text);
            E('el5').disabled = false;
        });
    };
};

// If user selected a file, read it into memory and trigger sendFileData()
document.getElementById('el1').onchange = function(ev) {
    f = ev.target.files[0];
    if (!f) return;
    document.getElementById('el4').innerText = f.name;
    document.getElementById('el5').removeAttribute('disabled');
};

function download(url) {
    //window.open(url, '_self');
    //window.location.href=url;

    let fileName='设备日志';
    const a = document.createElement('a');
    a.style.display = 'none';
    a.setAttribute('target', '_blank');
    /*
     * download的属性是HTML5新增的属性
     * href属性的地址必须是非跨域的地址，如果引用的是第三方的网站或者说是前后端分离的项目(调用后台的接口)，这时download就会不起作用。
     * 此时，如果是下载浏览器无法解析的文件，例如.exe,.xlsx..那么浏览器会自动下载，但是如果使用浏览器可以解析的文件，比如.txt,.png,.pdf....浏览器就会采取预览模式
     * 所以，对于.txt,.png,.pdf等的预览功能我们就可以直接不设置download属性(前提是后端响应头的Content-Type: application/octet-stream，如果为application/pdf浏览器则会判断文件为 pdf ，自动执行预览的策略)
     */
    fileName && a.setAttribute('download', fileName);
    a.href = url;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
}
E('download_btn').onclick = ev => download('device.log');


const ChangePassword = function (props) {
    const [user, setUser] = useState('');
    const [pass, setPass] = useState('');
    const [newpass, setNewPass] = useState('');
    const login = () =>
        fetch(
              '/api/changepassword',
              { headers: { Authorization: 'Basic ' + btoa(user + ':' + pass) } })
            .then(r => {if(r.status == 403)alert('user or password wrong!');return r.json()})
            .then(r => r && props.login(r))
            .catch(err => err);
    return html`
<div class="rounded border" style="max-width: 480px; margin: 0 auto; margin-top: 5em; background: #eee; ">
  <div style="padding: 2em; ">
    <h1 style="color: #666;">IPC Web Settings Login </h1>
    <div style="margin: 0.5em 0;">
      <input type='text' placeholder='User' style="width: 100%;"
        oninput=${ev => setUser(ev.target.value)} value=${user} />
    </div>
    <div style="margin: 0.5em 0;">
      <input type="password" placeholder="Password" style="width: 100%;"
        oninput=${ev => setPass(ev.target.value)} value=${pass}
        onchange=${login} />
    </div>
    <div style="margin: 0.5em 0;">
      <input type="password" placeholder="Password" style="width: 100%;"
        oninput=${ev => setNewPass(ev.target.value)} value=${newpass}
        onchange=${login} />
    </div>
    <div style="margin: 1em 0;">
      <button class="btn" style="width: 100%; background: #8aa;"
        disabled=${!user || !pass} onclick=${login}> Login </button>
    </div>
    <div style="margin: 1em 0;">
      <button class="btn" style="width: 100%; background: #8aa;"
        disabled=${!user || !pass} onclick=${login}> Login </button>
    </div>
    <div style="color: #777; margin-top: 2em;">
      Valid logins: admin:pass0, user1:pass1, user2:pass2
    </div>
  </div>
</div>`;
};

const Nav = props => html`
<div style="background: #333; padding: 0.5em; color: #fff;">
  <div class="container d-flex">
    <div style="flex: 1 1 auto; display: flex; align-items: center;">
      <b>IPC Web Settings</b>
    </div>
    <div style="display: flex; align-items: center; flex: 0 0 auto; ">
      <span>Logged in as:</span>
      <span style="padding: 0 0.5em;"><img src="user.png" height="22" /></span>
      <span>${props.user}</span>
      <a class="btn" onclick=${props.logout}
        style="margin-left: 1em; font-size: 0.8em; background: #8aa;">logout</a>
    </div>
  </div>
</div>`;


const Login = function (props) {
    const [user, setUser] = useState('');
    const [pass, setPass] = useState('');
    const change_pwd = () => {window.location.href='change_pwd.html';}
    const login = () =>
        fetch(
              '/api/login',
              { headers: { Authorization: 'Basic ' + btoa(user + ':' + pass) } })
            .then(r => {if(r.status == 403)alert('user or password wrong!');return r.json()})
            .then(r => r && props.login(r))
            .catch(err => err);
    return html`
<div class="rounded border" style="max-width: 480px; margin: 0 auto; margin-top: 5em; background: #eee; ">
  <div style="padding: 2em; ">
    <h1 style="color: #666;">IPC Web Settings Login </h1>
    <div style="margin: 0.5em 0;">
      <input type='text' placeholder='Name' style="width: 100%;"
        oninput=${ev => setUser(ev.target.value)} value=${user} />
    </div>
    <div style="margin: 0.5em 0;">
      <input type="password" placeholder="Password" style="width: 100%;"
        oninput=${ev => setPass(ev.target.value)} value=${pass} />
    </div>
    <div style="margin: 1em 0;">
      <button class="btn" style="width: 100%; background: #8aa;"
        disabled=${!user || !pass} onclick=${login}> Login </button>
    </div>
    <div style="display: none;">
      <button style="width: 40%; "
        onclick=${change_pwd}> Change Password </button>
    </div>
    <div style="margin: 1em 0;">
      <a href='change_pwd.html'>Change Password</a>
    </div>
    <div style="color: #777; margin-top: 2em;">
      Valid logins: admin:pass0
    </div>
  </div>
</div>`;
};

const App = function () {
    const [user, setUser] = useState('');
    const [data, setData] = useState({});

    const getin = () =>
        fetch('/api/data', { headers: { Authorization: '' } })
            .then(r => r.json())
            .then(r => setData(r))
            .catch(err => console.log(err));

    const login = function (u) {
        document.cookie = `access_token=${u.token}; Secure, HttpOnly; SameSite=Lax; path=/; max-age=3600`;
        setUser(u.user);
        return getin();
    };

    const logout = () => {
        document.cookie = `access_token=; Secure, HttpOnly; SameSite=Lax; path=/; max-age=0`;
        setUser('');
    };

    useEffect(() => {
        // Called once at init time
        fetch('/api/login', { headers: { Authorization: '' } })
            .then(r => r.json())
            .then(r => login(r))
            .catch(() => setUser(''));
    }, []);

    if (!user){
        E('main_page').style.display = "none";
        return html`<${Login} login=${login} />`;
    }

    /*
  return html`
<${Nav} user=${user} logout=${logout} />
<${Main} data=${data} />
`;
    if (!user) {
        window.location.href='login.html';
    }
    */

    E('main_page').style.display = "block";
    test_interface();
    return html`<${Nav} user=${user} logout=${logout} />`;// + main_page;
    //return main_page;
};


//window.onload = () => render(h(App), document.body);
window.onload = () => render(h(App), E('login'));
/*window.onload = function(){
    test_interface();
}*/


