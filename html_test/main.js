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

async function test_interface(){
    //window.location.href="http://127.0.0.1:8000/web_root";

    //fetch('/api/sum', {method: 'POST', body:JSON.stringify([1, 2])});
    //try {
    let response = await fetch('/api/fn3', {method: 'POST', body:JSON.stringify([1, 2])});
    let json = await response.json();
    if (response.status == 200) {
        log('get location ' + JSON.stringify(json));

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
  const login = () =>
    fetch(
      '/api/login',
      { headers: { Authorization: 'Basic ' + btoa(user + ':' + pass) } })
      .then(r => r.json())
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
        oninput=${ev => setPass(ev.target.value)} value=${pass}
        onchange=${login} />
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


