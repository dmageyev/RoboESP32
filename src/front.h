// ==================== ВЕБ-ІНТЕРФЕЙС (HTML/JS) ====================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>NETRUNNER OS v4.0 // UNSTABLE</title>
    <style>
        :root { 
            --deep-space: #020204; 
            --neon-pink: #ff0055; 
            --neon-green: #39ff14; 
            --neon-cyan: #00f0ff;
            --glass-bg: rgba(6, 6, 10, 0.82);
            --border-glow: rgba(0, 240, 255, 0.12);
        }
        
        * { box-sizing: border-box; margin: 0; padding: 0; user-select: none; -webkit-user-select: none; font-family: 'Orbitron', 'Segoe UI', monospace; letter-spacing: 1px; }
        
        body { 
            background: radial-gradient(circle at center, #0f0a1c 0%, var(--deep-space) 100%);
            color: #e2e8f0; 
            overflow: hidden; 
            height: 100vh; 
            display: flex; 
            flex-direction: column; 
            position: relative;
        }

        /* --- ПЕРІОДИЧНЕ ПОСМИКУВАННЯ ВСЬОГО ЕКРАНУ (ГЛІТЧ СИСТЕМИ) --- */
        body.glitch-shake {
            animation: screen-shake 0.25s linear infinite;
        }
        @keyframes screen-shake {
            0% { transform: translate(0, 0) skewX(0deg); filter: hue-rotate(0deg); }
            10% { transform: translate(-2px, 1px) skewX(-1deg); filter: contrast(1.2); }
            20% { transform: translate(2px, -1px) skewX(1deg); }
            30% { transform: translate(0, 0) skewX(0deg); }
            100% { transform: translate(0, 0) skewX(0deg); }
        }

        /* --- ВИПАДКОВІ НЕОНОВІ ЗАВАДИ ТА СМУГИ ШУМУ --- */
        .noise-layer {
            position: fixed; top: 0; left: 0; width: 100vw; height: 100vh;
            pointer-events: none; z-index: 990; display: none;
            background: linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.25) 50%), linear-gradient(90deg, rgba(255, 0, 0, 0.06), rgba(0, 255, 0, 0.02), rgba(0, 0, 255, 0.06));
            background-size: 100% 4px, 6px 100%;
        }
        .noise-line {
            position: fixed; left: 0; width: 100vw; height: 2px;
            background: rgba(0, 240, 255, 0.3); box-shadow: 0 0 10px var(--neon-cyan);
            pointer-events: none; z-index: 991; display: none;
        }

        /* --- ІМІТАЦІЯ СПРОБИ ВЗЛОМУ (ФОНОВИЙ ХАК) --- */
        #hacker-alert {
            position: fixed; top: 30%; left: 50%; transform: translate(-50%, -50%);
            color: rgba(255, 0, 85, 0.15); font-size: 28px; font-weight: bold;
            text-transform: uppercase; text-align: center; pointer-events: none;
            z-index: 1; display: none; font-family: monospace;
            animation: alert-blink 0.2s infinite alternate;
        }
        @keyframes alert-blink { 0% { opacity: 0; } 100% { opacity: 1; text-shadow: 0 0 10px var(--neon-pink); } }

        /* --- ЕКРАН ЗАВАНТАЖЕННЯ (ЗБІЛЬШЕНИЙ ЧАС) --- */
        #boot-screen {
            position: fixed; top: 0; left: 0; width: 100vw; height: 100vh;
            background: #000; z-index: 999; padding: 25px;
            display: flex; flex-direction: column; justify-content: flex-start;
            font-family: monospace; font-size: 11px; color: var(--neon-green);
            overflow: hidden;
        }
        .log-line { margin-bottom: 3px; opacity: 0; transform: translateY(3px); animation: show-line 0.05s forwards; }
        .log-cyan { color: var(--neon-cyan); }
        .log-pink { color: var(--neon-pink); }
        @keyframes show-line { to { opacity: 1; transform: translateY(0); } }
        
        .boot-progress { width: 250px; height: 3px; border: 1px solid var(--neon-green); margin-top: 20px; position: relative; }
        .boot-bar { width: 0%; height: 100%; background: var(--neon-green); box-shadow: 0 0 10px var(--neon-green); animation: load-bar 4.8s cubic-bezier(0.2, 0.6, 0.4, 1) forwards; }
        @keyframes load-bar { to { width: 100%; } }

        /* --- ЕКРАН ВТРАТИ ЗВ'ЯЗКУ --- */
        #disconnect-screen {
            position: fixed; top: 0; left: 0; width: 100vw; height: 100vh;
            background: rgba(10, 1, 4, 0.93); backdrop-filter: blur(10px);
            z-index: 998; display: none; flex-direction: column;
            justify-content: center; align-items: center; border: 3px solid var(--neon-pink);
            box-shadow: inset 0 0 50px rgba(255, 0, 85, 0.6);
        }
        .error-title { font-size: 22px; color: var(--neon-pink); text-shadow: 0 0 15px var(--neon-pink); font-weight: bold; animation: glitch-text 0.2s infinite alternate; }
        @keyframes glitch-text { 0% { transform: skewX(-4deg) scale(0.98); } 100% { transform: skewX(4deg) scale(1.02); text-shadow: -3px 0 red, 3px 0 cyan; } }

        /* --- МІРГОТІННЯ ПІДСВІТКИ (НЕСТАБІЛЬНИЙ НЕОН) --- */
        .card, header, .btn-dir {
            animation: neon-flicker 6s infinite linear;
        }
        @keyframes neon-flicker {
            0%, 48%, 50%, 52%, 74%, 76%, 100% { opacity: 1; filter: drop-shadow(0 0 0px transparent); }
            49%, 75% { opacity: 0.85; filter: brightness(0.7) contrast(1.2); }
            51% { opacity: 0.95; }
        }

        /* --- ГОЛОВНИЙ ІНТЕРФЕЙС --- */
        header { 
            background: rgba(3, 3, 5, 0.96); padding: 10px 20px; display: flex; justify-content: space-between; align-items: center; 
            border-bottom: 2px solid var(--neon-pink); box-shadow: 0 0 20px rgba(255, 0, 85, 0.3); height: 45px; z-index: 10;
            opacity: 0; transform: translateY(-50px);
        }
        .main-layout { flex: 1; display: grid; grid-template-columns: 1fr 1.2fr; padding: 12px; gap: 12px; height: calc(100vh - 45px); overflow: hidden; }
        
        /* РОЗГОРТАННЯ ПАНЕЛЕЙ */
        .control-panel { 
            opacity: 0; transform: scale(0.8) rotateX(20deg);
            background: var(--glass-bg); backdrop-filter: blur(12px); border: 1px solid var(--border-glow);
            display: flex; justify-content: center; align-items: center; padding: 20px 10px; position: relative;
            clip-path: polygon(0 0, calc(100% - 20px) 0, 100% 20px, 100% 100%, 20% 100%, 0 calc(100% - 20px));
        }
        .side-panel { display: flex; flex-direction: column; gap: 10px; overflow: hidden; }
        .card { opacity: 0; transform: translateY(80px) cubic-bezier(0.175, 0.885, 0.32, 1.275); background: var(--glass-bg); backdrop-filter: blur(12px); border: 1px solid var(--border-glow); padding: 12px 14px; position: relative; }

        .system-ready header { opacity: 1; transform: translateY(0); transition: all 0.5s ease-out; }
        .system-ready .control-panel { opacity: 1; transform: scale(1) rotateX(0); transition: all 0.7s cubic-bezier(0.15, 0.85, 0.3, 1) 0.1s; }
        .system-ready .card:nth-child(1) { opacity: 1; transform: translateY(0); transition: all 0.5s cubic-bezier(0.15, 0.85, 0.3, 1) 0.3s; }
        .system-ready .card:nth-child(2) { opacity: 1; transform: translateY(0); transition: all 0.5s cubic-bezier(0.15, 0.85, 0.3, 1) 0.4s; }
        .system-ready .card:nth-child(3) { opacity: 1; transform: translateY(0); transition: all 0.5s cubic-bezier(0.15, 0.85, 0.3, 1) 0.5s; }

        /* Елементи пульту */
        header h2 { font-size: 13px; color: var(--neon-cyan); text-shadow: 0 0 10px var(--neon-cyan); }
        .status-badge { padding: 3px 12px; font-size: 10px; font-weight: bold; border: 1px solid var(--neon-pink); color: var(--neon-pink); clip-path: polygon(8px 0%, 100% 0%, calc(100% - 8px) 100%, 0% 100%); }
        .status-badge.connected { border-color: var(--neon-green); color: var(--neon-green); text-shadow: 0 0 5px var(--neon-green); }
        
        .d-pad { display: grid; grid-template-columns: repeat(3, 64px); grid-template-rows: repeat(3, 64px); gap: 10px; z-index: 2; }
        .btn-dir { 
            background: rgba(15, 23, 42, 0.5); border: 1px solid var(--neon-cyan); color: var(--neon-cyan); font-size: 22px; cursor: pointer; 
            display: flex; justify-content: center; align-items: center; touch-action: manipulation; transition: all 0.1s ease;
            clip-path: polygon(15% 0%, 85% 0%, 100% 15%, 100% 85%, 85% 100%, 15% 100%, 0% 85%, 0% 15%);
        }
        .btn-dir:active { background: var(--neon-cyan); color: #000; box-shadow: 0 0 25px var(--neon-cyan); transform: scale(0.95); }
        #btn-stop { background: rgba(255, 0, 85, 0.1); border-color: var(--neon-pink); color: var(--neon-pink); }
        #btn-stop:active { background: var(--neon-pink); color: #000; box-shadow: 0 0 30px var(--neon-pink); }

        .card::before { content: ''; position: absolute; top: -1px; left: 15px; width: 35px; height: 2px; background: var(--neon-cyan); box-shadow: 0 0 8px var(--neon-cyan); }
        .card h3 { margin-bottom: 8px; font-size: 10px; color: #4b5563; text-transform: uppercase; }
        
        .slider-group { margin-bottom: 6px; }
        .slider-label { display: flex; justify-content: space-between; margin-bottom: 3px; font-size: 10px; color: #9ca3af;}
        .slider-label span:last-child { color: var(--neon-cyan); }
        
        input[type=range] { width: 100%; height: 4px; background: #0b0b12; outline: none; -webkit-appearance: none; border: 1px solid #1f2937; }
        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 8px; height: 18px; background: var(--neon-cyan); cursor: pointer; box-shadow: 0 0 10px var(--neon-cyan); }
        
        .grid-custom { display: grid; grid-template-columns: repeat(2, 1fr); gap: 8px; }
        .grid-safety { display: grid; grid-template-columns: repeat(3, 1fr); gap: 8px; }
        
        .btn-toggle { background: rgba(10, 10, 16, 0.6); color: #4b5563; border: 1px solid #1f2937; padding: 10px 2px; font-size: 9px; font-weight: bold; cursor: pointer; text-transform: uppercase; clip-path: polygon(0 0, calc(100% - 6px) 0, 100% 6px, 100% 100%, 6px 100%, 0 calc(100% - 6px)); transition: all 0.2s; }
        .btn-toggle[data-safety="brake"].active { background: rgba(255, 0, 85, 0.15); color: var(--neon-pink); border-color: var(--neon-pink); box-shadow: 0 0 15px rgba(255, 0, 85, 0.4); }
        .btn-toggle[data-safety="hazard"].active { background: rgba(255, 0, 150, 0.15); color: #ff00aa; border-color: #ff00aa; box-shadow: 0 0 15px rgba(255, 0, 150, 0.4); }
        .btn-toggle[data-light="head"].active { background: rgba(0, 240, 255, 0.15); color: var(--neon-cyan); border-color: var(--neon-cyan); box-shadow: 0 0 15px rgba(0, 240, 255, 0.4); }
        .btn-toggle[data-light="tail"].active { background: rgba(255, 0, 85, 0.15); color: var(--neon-pink); border-color: var(--neon-pink); box-shadow: 0 0 15px rgba(255, 0, 85, 0.4); }
        .btn-toggle[data-light="left"].active, .btn-toggle[data-light="right"].active { background: rgba(57, 255, 20, 0.15); color: var(--neon-green); border-color: var(--neon-green); box-shadow: 0 0 15px rgba(57, 255, 20, 0.4); }
        
        .btn-horn { background: rgba(15, 23, 42, 0.3); color: #9ca3af; border: 1px solid #1f2937; padding: 10px 2px; font-size: 9px; font-weight: bold; cursor: pointer; text-transform: uppercase; clip-path: polygon(0 0, calc(100% - 6px) 0, 100% 6px, 100% 100%, 6px 100%, 0 calc(100% - 6px)); }
        .btn-horn:active { background: var(--neon-green); color: #000; border-color: var(--neon-green); box-shadow: 0 0 20px var(--neon-green); }

        @media (max-width: 768px) and (orientation: portrait) { .main-layout { grid-template-columns: 1fr; grid-template-rows: auto 1fr; overflow: hidden; } .side-panel { overflow-y: auto !important; padding-bottom: 20px; } }
        @media (max-width: 950px) and (orientation: landscape) { .main-layout { grid-template-columns: 1fr 1.4fr; } .side-panel { display: grid !important; grid-template-columns: 1fr 1fr; gap: 10px !important; } .side-panel .card:first-child { grid-column: span 2; } }
    </style>
</head>
<body>

    <div class="noise-layer" id="screen-noise"></div>
    <div class="noise-line" id="noise-line"></div>
    <div id="hacker-alert">[ WARNING: ICE_BREAKER_ATTACK DETECTED ]<br><span style="font-size:12px;color:#aaa;">OVERRIDING MATRIX CONTROLS...</span></div>

    <div id="boot-screen">
        <div id="log-container"></div>
        <div class="boot-progress"><div class="boot-bar"></div></div>
    </div>

    <div id="disconnect-screen">
        <div class="error-title">CRITICAL_ERROR: LINK_LOST</div>
        <div style="font-size:11px; margin-top:10px; color:#a1a1aa; letter-spacing:2px;">RECONNECTING TO MATRIX MODULE...</div>
    </div>

    <header>
        <h2>// UNSTABLE_NETRUNNER_OS: v4.0</h2>
        <div id="status" class="status-badge">LINK_DOWN</div>
    </header>
    
    <div class="main-layout">
        <div class="control-panel">
            <div class="d-pad">
                <div></div><button class="btn-dir" id="btn-up" data-dir="F">▲</button><div></div>
                <button class="btn-dir" id="btn-left" data-dir="L">◀</button><button class="btn-dir" id="btn-stop" data-dir="S">🛑</button><button class="btn-dir" id="btn-right" data-dir="R">▶</button>
                <div></div><button class="btn-dir" id="btn-down" data-dir="B">▼</button><div></div>
            </div>
        </div>
        <div class="side-panel">
            <div class="card">
                <h3>[ SYSTEM_OVERRIDE ]</h3>
                <div class="grid-safety">
                    <button class="btn-toggle" data-safety="brake">P-LOCK</button>
                    <button class="btn-toggle" data-safety="hazard">BEACON</button>
                    <button class="btn-horn" id="btn-horn">🔊 SONIC</button>
                </div>
            </div>
            <div class="card">
                <h3>[ KINETIC_FORCE_PROJECTION ]</h3>
                <div class="slider-group">
                    <div class="slider-label"><span>THRUST_VELOCITY</span><span id="speed-val">200</span></div>
                    <input type="range" id="slider-speed" min="0" max="255" value="200">
                </div>
                <div class="slider-group">
                    <div class="slider-label"><span>YAW_STABILIZER</span><span id="balance-val">0</span></div>
                    <input type="range" id="slider-balance" min="-50" max="50" value="0">
                </div>
            </div>
            <div class="card">
                <h3>[ LIGHT_EMISSION_CHANNELS ]</h3>
                <div class="grid-custom">
                    <button class="btn-toggle" data-light="head">MATRIX_HEAD</button>
                    <button class="btn-toggle" data-light="tail">REAR_GLOW</button>
                    <button class="btn-toggle" id="btn-turn-l" data-light="left">INDEX_L</button>
                    <button class="btn-toggle" id="btn-turn-r" data-light="right">INDEX_R</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        const gateway = `ws://${window.location.hostname}/ws`;
        let websocket;
        const discScreen = document.getElementById('disconnect-screen');

        // Подовжений та деталізований Системний Лог (~5 секунд завантаження)
        const logs = [
            {t: "BOOTING NETRUNNER CORRUPTED KERNEL...", c: ""},
            {t: "LOADING MATRIX PROXIES: [OK]", c: "cyan"},
            {t: "CONNECTING HOST WIFI TRANSCEIVER...", c: ""},
            {t: "ALERT: MULTIPLE PORT SCANS DETECTED!", c: "pink"},
            {t: "INJECTING WEBSOCKET PROTOCOLS... [STABLE]", c: "cyan"},
            {t: "BYPASSING MILITECH HARD ICE...", c: "pink"},
            {t: "EXTRACTING ENCRYPTION KEYS...", c: ""},
            {t: "DECRYPTING NODE CONTROLS: 25%... 68%... 100%", c: "cyan"},
            {t: "WARNING: FIREWALL CORRUPTION DETECTED", c: "pink"},
            {t: "ESTABLISHING TELEMETRY TUNNEL [OVERRIDE]", c: ""},
            {t: "UPLINK STABILIZED. CONTROLS DEPLOYED.", c: "cyan"}
        ];
        
        const logContainer = document.getElementById('log-container');
        logs.forEach((log, index) => {
            setTimeout(() => {
                const p = document.createElement('div');
                p.className = `log-line ${log.c ? 'log-' + log.c : ''}`;
                p.textContent = `> ${log.t}`;
                logContainer.appendChild(p);
            }, index * 420);
        });

        // Запуск інтерфейсу після завантаження
        setTimeout(() => {
            const boot = document.getElementById('boot-screen');
            boot.style.transition = "opacity 0.4s ease";
            boot.style.opacity = "0";
            setTimeout(() => {
                boot.style.display = "none";
                document.body.classList.add('system-ready');
                initWebSocket();
                startGlitchEngine(); // Вмикаємо двигун завад і збоїв
            }, 400);
        }, 5000);

        // --- ДВИГУН ВИПАДКОВИХ ЗАВАД ТА ЗЛАМІВ (ГЛІТЧІ) ---
        function startGlitchEngine() {
            const noiseLayer = document.getElementById('screen-noise');
            const noiseLine = document.getElementById('noise-line');
            const hackAlert = document.getElementById('hacker-alert');

            setInterval(() => {
                const rand = Math.random();
                
                // 1. Випадкове сіпання екрану (15% шанс)
                if (rand < 0.15) {
                    document.body.classList.add('glitch-shake');
                    setTimeout(() => document.body.classList.remove('glitch-shake'), 250);
                }
                
                // 2. Поява горизонтальної смуги перешкод (20% шанс)
                if (rand > 0.15 && rand < 0.35) {
                    noiseLine.style.top = Math.floor(Math.random() * 100) + 'vh';
                    noiseLine.style.display = 'block';
                    noiseLayer.style.display = 'block';
                    setTimeout(() => {
                        noiseLine.style.display = 'none';
                        noiseLayer.style.display = 'none';
                    }, 120);
                }

                // 3. Імітація спроби зовнішнього зламу (5% шанс)
                if (rand > 0.95) {
                    hackAlert.style.display = 'block';
                    document.body.classList.add('glitch-shake');
                    setTimeout(() => {
                        hackAlert.style.display = 'none';
                        document.body.classList.remove('glitch-shake');
                    }, 1200);
                }
            }, 1000);
        }

        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = () => { 
                const s = document.getElementById('status'); s.textContent = "LINK_ACTIVE"; s.classList.add('connected');
                discScreen.style.display = "none"; 
            }; 
            websocket.onclose = () => { 
                const s = document.getElementById('status'); s.textContent = "LINK_DOWN"; s.classList.remove('connected'); 
                discScreen.style.display = "flex"; 
                setTimeout(initWebSocket, 2000); 
            }; 
        }

        function sendCommand(data) { if (websocket && websocket.readyState === WebSocket.OPEN) websocket.send(JSON.stringify(data)); }

        const sSp = document.getElementById('slider-speed'); const sBl = document.getElementById('slider-balance');
        sSp.oninput = () => document.getElementById('speed-val').textContent = sSp.value;
        sBl.oninput = () => document.getElementById('balance-val').textContent = sBl.value;
        
        document.querySelectorAll('.btn-dir').forEach(b => {
            b.addEventListener('pointerdown', () => sendCommand({ type: "move", dir: b.getAttribute('data-dir'), speed: parseInt(sSp.value), balance: parseInt(sBl.value) }));
            b.addEventListener('pointerup', () => { if (b.id !== 'btn-stop') sendCommand({ type: "move", dir: "S", speed: 0, balance: 0 }); });
        });

        document.querySelectorAll('.btn-toggle').forEach(b => { 
            b.addEventListener('click', () => { 
                const type = b.getAttribute('data-light');
                if (type === 'left' && !b.classList.contains('active')) {
                    const rightBtn = document.getElementById('btn-turn-r');
                    if (rightBtn.classList.contains('active')) { rightBtn.classList.remove('active'); sendCommand({ type: "light", name: "right", state: false }); }
                }
                if (type === 'right' && !b.classList.contains('active')) {
                    const leftBtn = document.getElementById('btn-turn-l');
                    if (leftBtn.classList.contains('active')) { leftBtn.classList.remove('active'); sendCommand({ type: "light", name: "left", state: false }); }
                }
                b.classList.toggle('active'); 
                const state = b.classList.contains('active');
                if(b.hasAttribute('data-light')) sendCommand({ type: "light", name: type, state: state });
                if(b.hasAttribute('data-safety')) sendCommand({ type: "safety", name: b.getAttribute('data-safety'), state: state });
            }); 
        });

        const h = document.getElementById('btn-horn');
        h.addEventListener('pointerdown', () => sendCommand({ type: "horn", state: true }));
        h.addEventListener('pointerup', () => sendCommand({ type: "horn", state: false }));
    </script>
</body>
</html>
)rawliteral";
