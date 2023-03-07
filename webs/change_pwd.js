'use strict';
import { h, html, render, useEffect, useState } from './preact.min.js';


var E = function(id) { return document.getElementById(id); };

const App = function () {
    const [user, setUser] = useState('');
    const [pass, setPass] = useState('');
    const [newpass, setNewPass] = useState('');
    const [checkpass, setCheckPass] = useState('');

    const Change = async function() {
        if (checkpass == newpass) {
            try {
                let r = await fetch(
                              '/api/changepassword',
                              { headers: { Authorization: 'Basic ' + btoa(user + ':' + pass)}, method: 'POST', body:btoa(newpass)});
                let text = await r.text();

                if(r.status == 403) {
                    alert('user or password wrong!');
                    E('old_pwd').value="";
                }
                else {
                    alert(text);
                }
                //alert(text);
                if (r.status == 200) {
                    window.location.href='index.html';
                }
            }
            catch(error) {
                alert('failed');
            }
        }
        else {
            alert('New and confirmed passwords are not the same!')
        }
    }
    //E('btn1').onclick = ev => {window.location.href='index.html'}
    /*
    E('btn1').onclick = ev => Change();
    E('btn1').disabled = !user || !pass;

    E('user').oninput = ev => setUser(ev.target.value);
    E('pass').oninput = ev => setPass(ev.target.value);
    E('newpass').oninput = ev => setNewPass(ev.target.value);
    E('newpass2').oninput = ev => setNewPass2(ev.target.value);
    */

    return html`
<div class="rounded border" style="max-width: 480px; margin: 0 auto; margin-top: 5em; background: #eee; ">
  <div style="padding: 2em; ">
    <h1 style="color: #666;">IPC Web Settings Login </h1>
    <div style="margin: 0.5em 0;">
      <input type='text' placeholder='User' style="width: 100%;"
        oninput=${ev => setUser(ev.target.value)} value=${user} />
    </div>
    <div style="margin: 0.5em 0;">
      <input id="old_pwd" type="password" placeholder="Old Password" style="width: 100%;"
        oninput=${ev => setPass(ev.target.value)} value=${pass}
        />
    </div>
    <div style="margin: 0.5em 0;">
      <input type="password" placeholder="New Password" style="width: 100%;"
        oninput=${ev => setNewPass(ev.target.value)} value=${newpass}
        />
    </div>
    <div style="margin: 0.5em 0;">
      <input type="password" placeholder="Check Password" style="width: 100%;"
        oninput=${ev => setCheckPass(ev.target.value)} value=${checkpass}
        />
    </div>
    <div style="margin: 1em 0;">
      <button class="btn" style="width: 100%; background: #8aa;"
        disabled=${!user || !pass || !newpass || !checkpass} onclick=${Change}>Change</button>
    </div>
  </div>
</div>`;

}

window.onload = () => render(h(App), document.body);
