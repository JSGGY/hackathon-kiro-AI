# ChronoClash — Visión de Producto

## Concepto

ChronoClash es un juego multijugador 2v2 de carreras en 3D con mecánica temporal. Dos equipos compiten en tiempo real dentro del mismo mundo/instancia tridimensional. El objetivo es alcanzar el portal de salida antes que el equipo rival. No hay combate directo con arma primaria; la interacción entre equipos se basa en habilidades temporales y peligros del entorno.

## Condición de victoria y derrota

- **Victoria:** tu equipo es el primero en que al menos un miembro cruce el portal de salida.
- **Derrota:** el equipo rival llega primero al portal.

## Mecánica Core — Habilidad Temporal (Time Bubble)

Cada jugador posee una habilidad temporal llamada **Time Bubble** que altera la velocidad del tiempo dentro de un área de efecto esférica alrededor del personaje.

### Activación

- Se activa **manteniendo presionado** un botón dedicado (ej. `SPACE`).
- Mientras se mantiene activa, el personaje **puede seguir moviéndose**, pero sufre una **penalización de velocidad** (ej. −30 % de velocidad base).
- Al soltar el botón, la burbuja se desactiva inmediatamente.

### Efecto

- Dentro del radio de la burbuja, el factor de tiempo local (`timeScale`) se reduce (ej. 0.3×).
- **Afecta a todo** dentro del radio: plataformas móviles, trampas, proyectiles del entorno, **personajes aliados y rivales**.
- La burbuja es una esfera 3D visible en el espacio; su radio se mide en unidades del mundo 3D.

### Energía y recarga

| Parámetro | Valor orientativo |
|-----------|-------------------|
| Energía máxima | 100 unidades |
| Consumo mientras activa | 25 u/s |
| Recarga pasiva (en movimiento) | 15 u/s |
| Recarga pasiva (quieto) | 0 u/s |

- La energía **solo se recarga mientras el personaje se mueve** y la habilidad está inactiva.
- Si la energía llega a 0, la burbuja se desactiva forzosamente y hay un cooldown breve (ej. 1 s) antes de poder reactivarla.

## Habilidades secundarias y muerte

- **Sin arma primaria.** Los jugadores no pueden disparar ni golpear directamente.
- El daño proviene de:
  - Habilidades secundarias (empujones, colocación de trampas temporales, etc. — a definir por personaje).
  - Objetos y peligros del entorno (sierra, láseres, caídas al vacío, aplastamiento por objetos 3D).
- **Muerte instantánea:** un solo impacto elimina al personaje. Al morir, el jugador reaparece en el último checkpoint alcanzado por su equipo.

## Game Loop (partida típica)

1. **Matchmaking/Lobby:** 4 jugadores se emparejan (2v2). Se les asigna una sala generada.
2. **Cuenta atrás:** 3-2-1, ambos equipos aparecen en el punto de inicio compartido.
3. **Carrera:** los equipos avanzan por salas/bloques encadenados hacia el portal, usando la Time Bubble para sortear obstáculos o interferir con el rival.
4. **Resolución:** el primer equipo en tocar el portal gana. Se registra el tiempo en el Leaderboard.
5. **Post-partida:** pantalla de resultados con tiempos y puntuaciones.

## Generación de niveles

- Los niveles se construyen encadenando **salas/bloques prediseñados** en un grafo lineal que termina en el portal.
- Cada sala es un volumen 3D con obstáculos estáticos y dinámicos que interactúan con `timeScale`.
- La cámara sigue al personaje en tercera persona (perspectiva 3D); la orientación espacial es parte del desafío de navegación.

## Identidad del jugador

- Autenticación vía **Google** exclusivamente (Sign In with Google).
- El perfil almacena: ID único (`sub` del token), nombre visible y puntuaciones.
- No se requiere gestión de contraseñas ni proveedores adicionales.

## Anti-cheat

- El **servidor** es la fuente de verdad para:
  - Cronómetro de partida.
  - Registro de checkpoints alcanzados.
  - Llegada al portal.
- El cliente no puede autoproclamar victoria ni manipular tiempos de Leaderboard.
