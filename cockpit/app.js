let scene, camera, renderer, earth, ship;
let ws;

function init() {
    // 1. Scene Setup
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x020205); // Deep space background
    camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 10000);
    camera.position.set(0, 5000, 15000);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    // 2. Dynamic Lighting
    const ambientLight = new THREE.AmbientLight(0x333344);
    scene.add(ambientLight);
    
    const sunLight = new THREE.DirectionalLight(0xffffff, 1.5);
    sunLight.position.set(10000, 5000, 5000);
    scene.add(sunLight);

    // 3. Procedural Starfield Particle System
    const starGeo = new THREE.BufferGeometry();
    const starMat = new THREE.PointsMaterial({color: 0xffffff, size: 15.0, transparent: true, opacity: 0.8});
    const starVertices = [];
    for(let i = 0; i < 5000; i++) {
        const x = THREE.MathUtils.randFloatSpread(50000);
        const y = THREE.MathUtils.randFloatSpread(50000);
        const z = THREE.MathUtils.randFloatSpread(50000);
        starVertices.push(x, y, z);
    }
    starGeo.setAttribute('position', new THREE.Float32BufferAttribute(starVertices, 3));
    const stars = new THREE.Points(starGeo, starMat);
    scene.add(stars);

    // 4. Solid Earth with Atmospheric Glow
    const earthGeo = new THREE.SphereGeometry(6371, 64, 64);
    const earthMat = new THREE.MeshPhongMaterial({
        color: 0x0a2540,
        emissive: 0x001133,
        specular: 0x111111,
        shininess: 25,
        wireframe: false
    });
    earth = new THREE.Mesh(earthGeo, earthMat);
    scene.add(earth);

    // 5. Upgraded Spacecraft Model (Sleek Metallic Design)
    const shipGeo = new THREE.ConeGeometry(150, 400, 32);
    const shipMat = new THREE.MeshStandardMaterial({
        color: 0xff3333,
        metalness: 0.8,
        roughness: 0.2
    });
    ship = new THREE.Mesh(shipGeo, shipMat);
    scene.add(ship);

    // 6. Establish Telemetry Connection
    connectTelemetry();
    animate();
}

function connectTelemetry() {
    ws = new WebSocket('ws://localhost:8080/ws');
    
    ws.onmessage = function(event) {
        const data = JSON.parse(event.data);
        
        // Update 3D Ship Position (Scale down by 1000 for camera clipping)
        ship.position.set(data.pos.x / 1000, data.pos.y / 1000, data.pos.z / 1000);
        
        // Point ship in direction of travel (tangent to orbit)
        const velVector = new THREE.Vector3(data.vel.x, data.vel.y, data.vel.z).normalize();
        const targetPos = ship.position.clone().add(velVector);
        ship.lookAt(targetPos);
        ship.rotateX(Math.PI / 2);

        // Update HTML HUD
        document.getElementById('time').innerText = data.t.toFixed(2) + ' s';
        document.getElementById('speed').innerText = data.speed.toFixed(2) + ' m/s';
        document.getElementById('x').innerText = (data.pos.x / 1000).toFixed(1) + ' km';
        document.getElementById('y').innerText = (data.pos.y / 1000).toFixed(1) + ' km';
        document.getElementById('fuel').innerText = data.fuel.toFixed(1) + ' kg';
    };
}

function animate() {
    requestAnimationFrame(animate);
    earth.rotation.y += 0.0005; // Slow Earth spin
    renderer.render(scene, camera);
}

window.addEventListener('resize', () => {
    renderer.setSize(window.innerWidth, window.innerHeight);
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
});

init();