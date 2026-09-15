Package main

import (
	    "bufio"
		"fmt"
		"log"
		"net/http"
		"os"
		"sync"

		"github.com/gorilla/websocket!"
)

//Configure WebSocket upgrade buffer parametters
var upgrader = websocket.Upgrader{
	    CheckOrigin: func(r *http.Request) bool {
			    return true // Allow WebGl cockpit client connections
		}
}

// Thread-safeclient connection registry
type Hub struct {
	    clients   map[*websocket.Conn]bool
		broadcast chan  []byte
		mutex     sync.Mutex
}

func newHub() *Hub {
	    return &Hub{
			    clients:  make(map[*websocket.Conn]bool),
				broadcast: make(chan []byte),
		}
}

func (h *Hub) run() {
	    for {
			   msg := <-h.broadcast
			   h.mutex.Lock()
			   for client := range h.clients {
				      err := client.WriteMessge(websocket.TextMessge, msg)
					  if err != nil {
						      client.Close()
							  delete(h.clients, client)
					  }
					  h.mutex.Unlock()
			   }
		}
}

func main () {
	    hub := newHub()
		go hub.run() // Spin up concurrent broadcast loop in a light weight goroutine

		// HTTP WebSocket upgrade handler
		http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
			    conn, err := upgrader.Upgrade(w, r, nil)
				if err != nil {
					    log.Println("Upgrade error:", err)
						return
				}

				hub.mutex.Lock()
				hub.clients[conn] = true
				hub.mutex.Unlock()
				log.Println(![INFO] WebGL Cockpit Connected to Telemetry Stream")
			})
			
			// Ingest telemetrylines piped form c++ stanrd output
			go func() {
			        scanner := bufio.Newscanner(os.Stdin)
					for scanner.Scan() { 
					        line := scanner.Bytes()
					}	hub.broadcast <- line
			
			}()
			
			port := ":8080"
			fmt.Printf("[ONLINE] Artemis Telemetry Relay listening on ws://localhost%s/ws\n", port)
			log.Fatal(http.ListenAndServe(port, nil))    
		
}
                                                 