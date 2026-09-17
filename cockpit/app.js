let scene, camera, renderer, earth, craftGroup;
let orbitLine, orbitPoints = [];
let ws;

const EARTH_RADIUS_KM = 6371;
const SCALE = 0.001; // 1 Three.js unit = 1 km

function init() {
    // 1. Scene & Cinematic Camera
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x01030a);

    camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 100000);
    camera.position.set(0, 4000, 16000);

    renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: "high-performance" });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.toneMapping = THREE.ACESFilmicToneMapping;
    document.body.appendChild(renderer.domElement);

    // 2. Realistic Space Lighting
    const ambient = new THREE.AmbientLight(0x1a2536, 0.6);
    scene.add(ambient);

    const sun = new THREE.DirectionalLight(0xffffff, 2.2);
    sun.position.set(20000, 8000, 10000);
    scene.add(sun);

    // 3. Realistic Earth
    buildRealisticEarth();

    // 4. Procedural Deep-Space Starfield
    buildStarfield();

    // 5. Build Artemis-Orion Spacecraft 3D Model
    buildOrionSpacecraft();

    // 6. Orbital Trajectory Path Tracer
    buildTrajectoryLine();

    // 7. Connect Telemetry
    connectTelemetry();
    animate();
}

function buildRealisticEarth() {
    const earthGeo = new THREE.SphereGeometry(EARTH_RADIUS_KM, 96, 96);
    const earthMat = new THREE.MeshPhongMaterial({
        color: 0x123d6a,
        emissive: 0x020b18,
        specular: 0x3388cc,
        shininess: 35,
        wireframe: false
    });
    earth = new THREE.Mesh(earthGeo, earthMat);
    scene.add(earth);

    // Atmospheric Glow Shell (Fresnel layer)
    const atmoGeo = new THREE.SphereGeometry(EARTH_RADIUS_KM + 120, 64, 64);
    const atmoMat = new THREE.MeshBasicMaterial({
        color: 0x00a2ff,
        transparent: true,
        opacity: 0.12,
        side: THREE.BackSide
    });
    const atmo = new THREE.Mesh(atmoGeo, atmoMat);
    scene.add(atmo);
}

function buildStarfield() {
    const starGeo = new THREE.BufferGeometry();
    const starCount = 6000;
    const positions = new Float32Array(starCount * 3);
    for (let i = 0; i < starCount * 3; i += 3) {
        positions[i] = THREE.MathUtils.randFloatSpread(80000);
        positions[i + 1] = THREE.MathUtils.randFloatSpread(80000);
        positions[i + 2] = THREE.MathUtils.randFloatSpread(80000);
    }
    starGeo.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    const starMat = new THREE.PointsMaterial({ color: 0xc9e5ff, size: 20, transparent: true, opacity: 0.85 });
    scene.add(new THREE.Points(starGeo, starMat));
}

function buildOrionSpacecraft() {
    craftGroup = new THREE.Group();

    const metalMat = new THREE.MeshStandardMaterial({ color: 0xdddddd, metalness: 0.85, roughness: 0.25 });
    const darkMat = new THREE.MeshStandardMaterial({ color: 0x222225, metalness: 0.4, roughness: 0.6 });
    const solarMat = new THREE.MeshStandardMaterial({ color: 0x0a2f7d, metalness: 0.9, roughness: 0.1 });

    // Orion Command Module (Capsule Cone)
    const capsuleGeo = new THREE.ConeGeometry(90, 110, 24);
    const capsule = new THREE.Mesh(capsuleGeo, metalMat);
    capsule.position.y = 55;
    craftGroup.add(capsule);

    // European Service Module (Cylinder base)
    const serviceGeo = new THREE.CylinderGeometry(85, 85, 120, 24);
    const serviceModule = new THREE.Mesh(serviceGeo, darkMat);
    serviceModule.position.y = -60;
    craftGroup.add(serviceModule);

    // 4 Deployed Solar Array Wings (X-Wing configuration of Orion)
    for (let i = 0; i < 4; i++) {
        const angle = (i * Math.PI / 2) + Math.PI / 4;
        const wingGeo = new THREE.BoxGeometry(220, 6, 40);
        const wing = new THREE.Mesh(wingGeo, solarMat);
        wing.position.set(Math.cos(angle) * 160, -60, Math.sin(angle) * 160);
        wing.rotation.y = angle;
        craftGroup.add(wing);
    }

    craftGroup.scale.set(0.6, 0.6, 0.6);
    scene.add(craftGroup);
}

function buildTrajectoryLine() {
    const maxPoints = 500;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(maxPoints * 3);
    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    
    const material = new THREE.LineBasicMaterial({
        color: 0x00d2ff,
        transparent: true,
        opacity: 0.6
    });
    orbitLine = new THREE.Line(geometry, material);
    scene.add(orbitLine);
}

function connectTelemetry() {
    ws = new WebSocket('ws://localhost:8080/ws');

    ws.onmessage = function (event) {
        const data = JSON.parse(event.data);

        // Convert state vectors to km
        const posX = data.pos.x * SCALE;
        const posY = data.pos.y * SCALE;
        const posZ = data.pos.z * SCALE;

        // Position Spacecraft
        craftGroup.position.set(posX, posY, posZ);

        // Orient craft to face velocity vector
        const velVec = new THREE.Vector3(data.vel.x, data.vel.y, data.vel.z).normalize();
        const lookTarget = craftGroup.position.clone().add(velVec);
        craftGroup.lookAt(lookTarget);
        craftGroup.rotateX(Math.PI / 2);

        // Trace orbital trajectory
        orbitPoints.push(new THREE.Vector3(posX, posY, posZ));
        if (orbitPoints.length > 500) orbitPoints.shift();

        const positions = orbitLine.geometry.attributes.position.array;
        let index = 0;
        for (let i = 0; i < orbitPoints.length; i++) {
            positions[index++] = orbitPoints[i].x;
            positions[index++] = orbitPoints[i].y;
            positions[index++] = orbitPoints[i].z;
        }
        orbitLine.geometry.attributes.position.needsUpdate = true;
        orbitLine.geometry.setDrawRange(0, orbitPoints.length);

        // Calculate True Altitude
        const distFromCore = Math.sqrt(posX*posX + posY*posY + posZ*posZ);
        const altitude = distFromCore - EARTH_RADIUS_KM;

        // Update NASA HUD Panels
        document.getElementById('time').innerText = data.t.toFixed(1) + ' s';
        document.getElementById('alt').innerText = altitude.toFixed(2) + ' km';
        document.getElementById('speed').innerText = data.speed.toFixed(2) + ' m/s';
        document.getElementById('x').innerText = posX.toFixed(1) + ' km';
        document.getElementById('y').innerText = posY.toFixed(1) + ' km';
        document.getElementById('z').innerText = posZ.toFixed(1) + ' km';
        document.getElementById('fuel').innerText = data.fuel.toFixed(1) + ' kg';
        document.getElementById('mass').innerText = (2500 + data.fuel).toFixed(1) + ' kg';

        const riskEl = document.getElementById('risk-val');
        if (altitude < 160) {
            riskEl.innerText = 'CRITICAL DECAY';
            riskEl.style.color = '#ff3344';
        } else if (altitude < 300) {
            riskEl.innerText = 'ELEVATED DRAG';
            riskEl.style.color = '#ffaa00';
        } else {
            riskEl.innerText = 'STABLE LEO';
            riskEl.style.color = '#00ff88';
        }
    };
}

function animate() {
    requestAnimationFrame(animate);
    earth.rotation.y += 0.0003;
    renderer.render(scene, camera);
}

window.addEventListener('resize', () => {
    renderer.setSize(window.innerWidth, window.innerHeight);
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
});

init();