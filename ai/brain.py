import asyncio
import websockets
import json
import math

# Earth Constants
EARTH_RADIUS = 6371000.0
KARMAN_LINE = 100000.0

async def overwatch_brain():
    uri = "ws://localhost:8080/ws"
    print("[SYSTEM] Artemis Autonomous AI Overwatch Initializing...")
    
    async with websockets.connect(uri) as websocket:
        print("[SUCCESS] Neural Link Established. Ingesting Telemetry...\n")
        
        while True:
            try:
                message = await websocket.recv()
                data = json.loads(message)
                
                x, y, z = data["pos"]["x"], data["pos"]["y"], data["pos"]["z"]
                speed = data["speed"]
                fuel = data["fuel"]
                mission_time = data["t"]
                
                distance_from_core = math.sqrt(x**2 + y**2 + z**2)
                altitude = distance_from_core - EARTH_RADIUS
                
                print(f"T+{mission_time:.1f}s | Alt: {altitude/1000:.2f} km | Vel: {speed:.2f} m/s | Fuel: {fuel} kg")
                
                if altitude < KARMAN_LINE:
                    print(">> [WARNING] ATMOSPHERIC RE-ENTRY DETECTED.")
                elif altitude < 250000.0 and speed < 7500.0:
                    print(">> [AI DIRECTIVE] Orbit decaying. Recommending burn.")
                elif fuel < 100:
                    print(">> [CRITICAL] Fuel reserves depleted.")
                    
            except websockets.exceptions.ConnectionClosed:
                break

if __name__ == "__main__":
    asyncio.run(overwatch_brain())