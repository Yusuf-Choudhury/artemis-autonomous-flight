import asyncio
import websockets
import json
import math
import time

EARTH_RADIUS = 6371000.0
KARMAN_LINE = 100000.0

async def overwatch_brain():
    uri = "ws://localhost:8080/ws"
    print("[SYSTEM] Artemis AI Overwatch Initializing...")
    
    async with websockets.connect(uri) as websocket:
        print("[SUCCESS] Neural Link Established. Ingesting Telemetry...\n")
        
        last_burn_time = 0.0  # Cooldown tracker
        
        while True:
            try:
                message = await websocket.recv()
                data = json.loads(message)
                
                x, y, z = data["pos"]["x"], data["pos"]["y"], data["pos"]["z"]
                speed = data["speed"]
                fuel = data["fuel"]
                mission_time = data["t"]
                
                altitude = math.sqrt(x**2 + y**2 + z**2) - EARTH_RADIUS
                
                print(f"T+{mission_time:.1f}s | Alt: {altitude/1000:.2f} km | Vel: {speed:.2f} m/s | Fuel: {fuel} kg")
                
                # AI DECISION MATRIX
                if altitude < KARMAN_LINE:
                    print(">> [WARNING] ATMOSPHERIC RE-ENTRY DETECTED.")
                elif altitude < 350000.0 and speed < 7700.0:  # If altitude drops below 350km
                    current_time = time.time()
                    if current_time - last_burn_time > 10.0:  # 10-second safety cooldown
                        print(">> [AI OVERRIDE] Orbit decaying. EXECUTING AUTONOMOUS BURN.")
                        
                        # Send Command to Go Server
                        command = json.dumps({"action": "BURN", "magnitude": 25.0})
                        await websocket.send(command)
                        
                        last_burn_time = current_time
                elif fuel < 100:
                    print(">> [CRITICAL] Fuel reserves depleted.")
                    
            except websockets.exceptions.ConnectionClosed:
                break

if __name__ == "__main__":
    asyncio.run(overwatch_brain())