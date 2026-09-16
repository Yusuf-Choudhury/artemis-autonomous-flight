package main

import (
	"bufio"
	"fmt"
	"log"
	"net/http"
	"os"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true },
}

type Hub struct {
	clients   map[*websocket.Conn]bool
	broadcast chan []byte
	mutex     sync.Mutex
}

func newHub() *Hub {
	return &Hub{
		clients:   make(map[*websocket.Conn]bool),
		broadcast: make(chan []byte),
	}
}

func (h *Hub) run() {
	for {
		msg := <-h.broadcast
		h.mutex.Lock()
		for client := range h.clients {
			err := client.WriteMessage(websocket.TextMessage, msg)
			if err != nil {
				client.Close()
				delete(h.clients, client)
			}
		}
		h.mutex.Unlock()
	}
}

// Enterprise Feature: Persistent Black Box Logging
func setupFlightLogger() *os.File {
	filename := fmt.Sprintf("flight_log_%d.json", time.Now().Unix())
	file, err := os.OpenFile(filename, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal("CRITICAL: Could not initialize Black Box logger:", err)
	}
	log.Println("[SYSTEM] Black Box Logger Initialized:", filename)
	return file
}

func main() {
	hub := newHub()
	go hub.run()

	// Initialize the database logger
	logFile := setupFlightLogger()
	defer logFile.Close()

	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			log.Println("Upgrade error:", err)
			return
		}

		hub.mutex.Lock()
		hub.clients[conn] = true
		hub.mutex.Unlock()
		log.Println("[INFO] WebGL Cockpit Connected - STREAMING LIVE")

		for {
			_, _, err := conn.ReadMessage()
			if err != nil {
				hub.mutex.Lock()
				delete(hub.clients, conn)
				hub.mutex.Unlock()
				conn.Close()
				log.Println("[WARN] WebGL Cockpit Disconnected")
				break
			}
		}
	})

	// Piped data listener and Database Writer
	go func() {
		scanner := bufio.NewScanner(os.Stdin)
		for scanner.Scan() {
			line := scanner.Bytes()

			// 1. Broadcast to WebGL Cockpit
			hub.broadcast <- line

			// 2. Write to persistent JSON log file
			logFile.Write(line)
			logFile.WriteString("\n")
		}
	}()

	port := ":8080"
	fmt.Printf("[ONLINE] Artemis Telemetry Relay listening on ws://localhost%s/ws\n", port)
	log.Fatal(http.ListenAndServe(port, nil))
}
