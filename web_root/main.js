'use strict';
import { h, html, render, useEffect, useState } from './preact.min.js';

const Nav = props => html`
<div style="background: #333; padding: 0.5em; color: #fff;">
  <div class="container d-flex">
    <div style="flex: 1 1 auto; display: flex; align-items: center;">
      <b>Your Product</b>
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

/*
const Main = props => html`
<div class="row">
  <H1>${props.data.text}</H1>
</div>`;
*/
var input1=1;
var input2=2;
//<div class="rounded border" style="max-width: 480px; margin: 0 auto; margin-top: 5em; background: #eee; ">
//btn2.title="Call SUM(${input1}, ${input2})"
var Main = props => html`
  <body>
    <div class="row">
        <H1>${props.data.text} REST basics demo</H1>
    </div>

  <center>
        <b>12.下拉选择表单</b>
        <br></br>
        选择你的幸运数字
        <br></br>
        <select id="select1">
            <option>114514</option>
            <option>2233</option>
            <option>777</option>
        </select>
        <br></br>
        <br></br>
        <br></br>
        <br></br>
        <br></br>
        <br></br>
        <br></br>
        <br></br>

    <div style="margin: 0.5em 0;">
      <input type='text' placeholder='input1' style="width: 10%;"
        value=${input1} />
    </div>
    <div style="margin: 0.5em 0;">
      <input type='text' placeholder='input2' style="width: 10%;"
        oninput="zsp_fun()" value=${input2} />
    </div>

    <button id="btn1">Call F1</button>
    <button id="btn2">Call SUM(${input1}, ${input2})</button>
    <button id="btn3">zsp test</button>
    <div style="margin-top: 1em;">Action log:</div>
    <div id="log" style="background: #eee; height: 10em; padding: 0.5em; overflow:auto;"></div>
  </center>
  </body>

  <script>
    var E = function(id) { return document.getElementById(id); };
    var btn1 = E('btn1'), btn2 = E('btn2'), msglog = E('log');
    var btn3 = E('btn3');
    var enable = function(en) { btn1.disabled = btn2.disabled = !en; };
    var log = text => msglog.innerHTML += text + '<br/>\n';

    enable(true);
    btn3.disabled = false;

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

    btn2.onclick = ev => getsum(${input1}, ${input2})
      .then(r => {
        console.log(r);
        log('POST [${input1}, ${input2}] to /api/sum: ' + JSON.stringify(r) + ' -> ' + JSON.stringify(r.result))
      })
      .catch(err => console.log(err));

    btn3.onclick = ev => fetch()
 function zsp_fun(){
        let x=document.getElementById('btn2');
        x.name="Call SUM(${input1}, ${input2})"
  }

  </script>
`;

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
    <h1 style="color: #666;">Your Product Login </h1>
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

  if (!user) return html`<${Login} login=${login} />`;

  return html`
<${Nav} user=${user} logout=${logout} />
<${Main} data=${data} />
`;
};

window.onload = () => render(h(App), document.body);
