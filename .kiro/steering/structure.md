# ChronoClash — Estructura de Proyecto

## Árbol de carpetas

```
chronoclash/
├── .kiro/
│   └── steering/
│       ├── product.md
│       ├── tech.md
│       └── structure.md
├── client/                          # Cliente C++/Raylib 3D (compilado a WASM)
│   ├── lib/
│   │   └── raylib/                  # Raylib como submódulo o copia local
│   │       └── src/
│   ├── src/
│   │   ├── main.cpp                 # Entry point, loop principal, init Raylib 3D
│   │   ├── shell.html               # Template HTML para Emscripten
│   │   ├── game/
│   │   │   ├── game.hpp             # Estado global de partida, structs compartidos
│   │   │   ├── game.cpp             # Lógica de game loop (update/draw 3D)
│   │   │   ├── player.hpp           # Clase Player: posición 3D, velocidad, estado
│   │   │   ├── player.cpp           # Movimiento 3D, física, checkpoints
│   │   │   ├── ability.hpp          # Clase Ability: Time Bubble
│   │   │   ├── ability.cpp          # Activación, energía, efecto de timeScale (esfera 3D)
│   │   │   ├── level.hpp            # Carga y gestión de bloques de nivel 3D
│   │   │   ├── level.cpp            # Parseo de JSON de bloques, instanciado de modelos
│   │   │   ├── hazards.hpp          # Definición de peligros del entorno 3D
│   │   │   ├── hazards.cpp          # Lógica de colisión 3D con peligros
│   │   │   └── camera.cpp           # Cámara en tercera persona, seguimiento del jugador
│   │   ├── net/
│   │   │   ├── net.hpp              # API de red (connect, send, receive)
│   │   │   ├── net.cpp              # WebSocket vía Emscripten (emscripten_websocket_*)
│   │   │   ├── messages.hpp         # Structs de mensajes (serialize/deserialize)
│   │   │   └── messages.cpp         # Encode/decode JSON de rutas WebSocket
│   │   ├── ui/
│   │   │   ├── ui.hpp               # API de interfaz (HUD, menús)
│   │   │   ├── ui.cpp               # Render de HUD 2D overlay (energía, timer, equipo)
│   │   │   └── screens.cpp          # Pantallas: lobby, countdown, results
│   │   └── utils/
│   │       ├── constants.hpp         # Constantes globales (velocidades, energía, radios, etc.)
│   │       └── json_parse.hpp        # Helper mínimo para parseo JSON
│   ├── assets/
│   │   ├── models/                  # Modelos 3D (.obj, .glb, .gltf)
│   │   ├── textures/                # Texturas para modelos y entorno
│   │   ├── shaders/                 # Shaders personalizados (bubble effect, etc.)
│   │   ├── sounds/                  # SFX y música
│   │   └── fonts/                   # Fuentes para UI
│   ├── build/                       # Output de compilación (gitignored)
│   │   ├── index.html
│   │   ├── index.js
│   │   ├── index.wasm
│   │   └── index.data               # Preloaded assets bundle (Emscripten)
│   └── Makefile                     # Targets: web, native, clean
├── backend/                         # Lambdas Node.js
│   ├── src/
│   │   ├── auth/
│   │   │   └── verify.js            # Verifica Google ID token, crea/actualiza User
│   │   ├── leaderboard/
│   │   │   ├── get.js               # GET /leaderboard — query top scores
│   │   │   └── post.js              # POST /leaderboard — (interno, llamado por ws/onMessage)
│   │   ├── ws/
│   │   │   ├── onConnect.js         # $connect: valida token, registra connectionId
│   │   │   ├── onDisconnect.js      # $disconnect: limpia connectionId de Rooms
│   │   │   └── onMessage.js         # Router de rutas WebSocket (room:*, ability:*, etc.)
│   │   ├── handlers/
│   │   │   ├── room.js              # Lógica room:join, room:ready, room:start
│   │   │   ├── ability.js           # Lógica ability:activate, ability:deactivate
│   │   │   ├── damage.js            # Lógica damage:hit → broadcast damage:death
│   │   │   ├── checkpoint.js        # Lógica checkpoint:reached → checkpoint:confirmed
│   │   │   └── portal.js            # Lógica portal:reached → portal:winner + leaderboard
│   │   └── shared/
│   │       ├── db.js                # Cliente DynamoDB, helpers de acceso a tablas
│   │       ├── broadcast.js         # Helper para enviar a todos los jugadores de una sala
│   │       ├── token.js             # Verificación de JWT (Google + session token propio)
│   │       └── constants.js         # Nombres de tablas, config compartida
│   ├── package.json
│   ├── template.yaml                # AWS SAM template (Lambdas + API Gateways + DynamoDB)
│   └── samconfig.toml               # Config de deploy SAM
├── levels/                          # Definiciones de bloques de nivel
│   ├── blocks/
│   │   ├── block_start.json         # Bloque de inicio (spawn point 3D)
│   │   ├── block_corridor_01.json   # Corredor con plataformas móviles
│   │   ├── block_hazard_01.json     # Sala con sierras/láseres
│   │   ├── block_vertical_01.json   # Sección vertical (escalada 3D)
│   │   └── block_portal.json        # Bloque final con el portal
│   └── sequences/
│       ├── sequence_easy.json       # Orden de bloques para dificultad fácil
│       └── sequence_medium.json     # Orden de bloques para dificultad media
├── docs/                            # Documentación adicional (opcional)
│   └── architecture.md             # Copia legible del diagrama para referencia rápida
├── .gitignore
└── README.md
```

## Convenciones de nombres

### Archivos C++ (cliente)

- **snake_case** para nombres de archivo: `player.cpp`, `time_bubble.cpp`, `net.cpp`.
- Un par `.hpp` / `.cpp` por módulo lógico (headers con extensión `.hpp` para distinguir de headers C puros).
- Clases en `PascalCase`: `Player`, `TimeBubble`, `Level`.
- Métodos en `camelCase`: `player.update()`, `ability.activate()`, `net.send()`.
- Constantes en `UPPER_SNAKE_CASE`: `MAX_ENERGY`, `BUBBLE_RADIUS`, `SPEED_PENALTY`.
- Namespaces en `snake_case`: `chrono_clash::game`, `chrono_clash::net`.

### Archivos JS (backend)

- **camelCase** para nombres de archivo: `onConnect.js`, `onMessage.js`, `verify.js`.
- Excepción: carpetas en **kebab-case** si hay más de una palabra (no aplica en la estructura actual).
- Funciones handler exportadas como `exports.handler = async (event) => { ... }`.
- Constantes en `UPPER_SNAKE_CASE` dentro de `constants.js`.

### Archivos JSON (niveles)

- **snake_case** con prefijo de tipo: `block_corridor_01.json`, `sequence_easy.json`.
- Numeración con dos dígitos (`_01`, `_02`) para orden natural en filesystem.
- Cada bloque define posiciones 3D (x, y, z), bounding boxes, y referencias a modelos.

### Rutas WebSocket

- Formato `dominio:acción` en **camelCase** tras los dos puntos: `room:join`, `ability:activate`, `portal:reached`.
- Mensajes del servidor al cliente usan el mismo formato: `ability:state`, `damage:death`, `portal:winner`.
- Posiciones se transmiten como `{x, y, z}` (coordenadas 3D del mundo).

### Tablas DynamoDB

- **PascalCase** para nombres de tabla: `Users`, `Rooms`, `Leaderboards`.
- **camelCase** para nombres de atributos: `userId`, `displayName`, `roomId`, `startedAt`.

### Assets 3D

- **kebab-case** para archivos de assets: `player-model.glb`, `laser-hum.wav`, `portal-glow.png`.
- Subcarpetas por tipo: `models/`, `textures/`, `shaders/`, `sounds/`, `fonts/`.
- Modelos en formato `.glb` o `.obj` (Raylib soporta ambos).
- Shaders en formato `.fs` / `.vs` (fragment/vertex GLSL).

## Notas de scaffold

- `client/build/` está en `.gitignore`; se genera con `make web` o los comandos de build descritos en `tech.md`.
- `client/lib/raylib/` se añade como submódulo Git (`git submodule add https://github.com/raysan5/raylib.git client/lib/raylib`) o como copia estática en el repo.
- El directorio `levels/` es consumido tanto por el backend (para enviar `levelSequence` a los clientes) como por el cliente (para instanciar modelos 3D según las posiciones definidas en cada bloque). Ambos lo leen como JSON.
- `backend/template.yaml` define todos los recursos AWS (tablas, funciones, APIs) como infraestructura-as-code.
- El build web usa `--preload-file assets` para empaquetar modelos 3D, texturas y sonidos en `index.data`.
- La cámara 3D en tercera persona está en `client/src/game/camera.cpp`; usa `Camera3D` de Raylib.
