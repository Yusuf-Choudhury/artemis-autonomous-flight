// 1. Scene, Camera, and Renderer Setup
const container = document.getElementById('canvas-container');
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 0.1, 1000);
const renderer = new THREE.WebGLRenderer({ antialias: true });

renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setPixelRatio(window.devicePixelRatio);
container.appendChild(renderer.domElement);

camera.position.set(0, 0, 20);

// 2. Lighting
const ambientLight = new THREE.AmbientLight(0x404040);
scene.add(ambientLight);
const sunLight = new THREE.DirectionalLight(0xffffff, 1.2);
sunLight.position.set(30, 20, 30);
scene.add(sunLight);

// 3. Central Body (Earth)
const earthGeometry = new THREE.SphereGeometry(6.371, 48, 48);
const earthMaterial = new THREE.MeshPhongMaterial({
    color: 0x1a5599,
    wireframe: true,
    emissive: 0x051122
});
const earth = new THREE.Mesh(earthGeometry, earthMaterial);
scene.add(earth);

// 4. Spacecraft (Artemis Orion)
const craftGeometry = new THREE.ConeGeometry(0.4, 1.0, 16);
const craftMaterial = new THREE.MeshBasicMaterial({ color: 0xff3344 });
const craft = new THREE.Mesh(craftGeometry, craftMaterial);
craft.position.set(0, 8.5, 0); // Default orbit altitude
scene.add(craft);

// 5. Connect to Go WebSocket Stream
const socket = new WebSocket('ws://localhost:8080/ws');

socket.onopen = () => {
    const statusEl = document.getElementById('status');
    if (statusEl) {
        statusEl.innerText = 'LINK ESTABLISHED (60Hz)';
        statusEl.style.color = '#00ff66';
    }
};

socket.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);

        // Update HUD Metrics
        document.getElementById('time').innerText = data.t.toFixed(2) + ' s';
        document.getElementById('speed').innerText = data.speed.toFixed(2) + ' m/s';
        document.getElementById('posX').innerText = (data.pos.x / 1000).toFixed(1) + ' km';
        document.getElementById('posY').innerText = (data.pos.y / 1000).toFixed(1) + ' km';
        document.getElementById('fuel').innerText = data.fuel.toFixed(1) + ' kg';

        // Scale physics position to 3D scene (1 unit = 1,000 km)
        const scale = 1.0 / 1000000.0;
        craft.position.set(data.pos.x * scale, data.pos.y * scale, data.pos.z * scale);
    } catch (err) {
        // Stream frame ignore
    }
};

socket.onerror = () => {
    const statusEl = document.getElementById('status');
    if (statusEl) {
        statusEl.innerText = 'WAITING FOR STREAM...';
        statusEl.style.color = '#ffaa00';
    }
};

// 6. 60 FPS Render Loop
function animate() {
    requestAnimationFrame(animate);
    earth.rotation.y += 0.002;
    renderer.render(scene, camera);
}

animate();

// Window Resize Handling
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});