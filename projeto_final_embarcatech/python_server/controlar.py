import curses, time
import paho.mqtt.client as mqtt

# ---------- CONFIG ----------
BROKER = "192.168.15.9"   # <-- troque para o IP do seu broker Mosquitto
PORT   = 1883
USER   = "aluno"
PASS   = "caiovitor12"

TOPIC_CMD   = "escola/sala1/carro/cmd"
TOPIC_SPEED = "escola/sala1/carro/speed"

# Modo 'hold': soltar tecla = enviar PARAR após X ms
HOLD_MODE = True
IDLE_MS   = 200

# Velocidade inicial (0..4095)
speed     = 3000

# ---------- MQTT ----------
client = mqtt.Client()
client.username_pw_set(USER, PASS)
client.connect(BROKER, PORT, 60)
client.loop_start()

def publish(topic, msg):
    client.publish(topic, msg, qos=0, retain=False)

def send_speed(val):
    global speed
    speed = max(0, min(4095, int(val)))
    publish(TOPIC_SPEED, str(speed))

last_cmd = "PARAR"
last_key_ts = 0.0

def send_cmd(cmd):
    global last_cmd, last_key_ts
    if cmd != last_cmd:
        publish(TOPIC_CMD, cmd)
        last_cmd = cmd
    last_key_ts = time.time()

# ---------- UI ----------
def main(stdscr):
    curses.curs_set(0)
    stdscr.nodelay(True)
    stdscr.keypad(True)

    stdscr.addstr(0,0,"Setas: mover | Espaço: Parar | +/-: velocidade | WASD: alternativa | q: sair")
    send_speed(speed)

    global last_cmd, last_key_ts
    while True:
        ch = stdscr.getch()
        now = time.time()

        if ch != -1:
            if ch == curses.KEY_UP or ch in (ord('w'), ord('W')):
                send_cmd("UP")
            elif ch == curses.KEY_DOWN or ch in (ord('s'), ord('S')):
                send_cmd("DOWN")
            elif ch == curses.KEY_LEFT or ch in (ord('a'), ord('A')):
                send_cmd("LEFT")
            elif ch == curses.KEY_RIGHT or ch in (ord('d'), ord('D')):
                send_cmd("RIGHT")
            elif ch == ord(' '):
                send_cmd("STOP")
            elif ch in (ord('+'), ord('=')):
                send_speed(speed + 150)
            elif ch == ord('-'):
                send_speed(speed - 150)
            elif ch in (ord('q'), ord('Q')):
                send_cmd("PARAR")
                break

        # Hold-mode: para se ficar sem tecla por X ms
        if HOLD_MODE and last_cmd != "PARAR" and (now - last_key_ts) * 1000 >= IDLE_MS:
            send_cmd("PARAR")

        stdscr.addstr(2,0,f"Velocidade: {speed:4d}   Último cmd: {last_cmd}        ")
        stdscr.refresh()
        time.sleep(0.02)

try:
    curses.wrapper(main)
finally:
    client.loop_stop()
    client.disconnect()
