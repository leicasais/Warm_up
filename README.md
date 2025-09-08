# EDA #level1: Orbital simulation

## Integrantes del grupo y contribución al trabajo de cada integrante

* [Nombre]: [contribución]

[completar]

## ...Verificación del timestep...

Para hacer las pruebas cambiamos los valores de timeMultiplier. En la primera prueba le asignamos 200 días por segundo de simulación, luego 400, 800, 1 000 y 10 000, y también comparamos contra 100 días por segundo. Observamos que el mejor desempeño lo obtuvimos con 100 días/seg: a medida que aumentábamos los días por segundo, las distancias Sol-planetas se volvían menos realistas y las órbitas se distorsionaban, expandiéndose o contrayéndose de forma artificial.

Esto se debe a que la fórmula que usamos para calcular las fuerzas de atracción gravitatoria utiliza una aproximación de Euler con una cota de error proporcional a Δt. En nuestro código Δt depende directamente de timeStep, por lo que, si aumenta la diferencia entre el primer tiempo de medida y el segundo, el error va a aumentar de manera proporcional, lo que genera una gran diferencia entre la distancia teórica de los planetas al Sol y la mostrada en la simulación. Por ejemplo, cuando hicimos la simulación asignándole a la variable el valor de 10 000, a 26.5 AU del Sol podíamos ver solo 3 planetas; en cambio, cuando la variable estaba en 100 se pueden ver 6 planetas a 11.5 AU.

### Visto matematicamente: 
Ecuación diferencial: d²r/dt² = -GM/r²
Método Euler: r(t+dt) = r(t) + v(t)×dt + ½×a(t)×dt²
Error por vuelta completa del bucle de simulaicon: ∝ dt³ (para posición)
Error acumulado: ∝ dt² × N_pasos


## ...Verificación del tipo de datos float...

Al relizar estas pruebas notamos que conviene usar doble precisión (`double`) en la física de la simulación (posiciones, velocidades, aceleraciones y tiempo) y dejar `float` solo para el render.

### Umbral de actualización de posición
Si el incremento es menor que la escala de redondeo (u) el cambio se pierde por redondeo (|v| * Δt  <  u * |r|) entonces el cambio de velocidad que toma el programa de un paso al otro se guarda como nulo por lo que genera una perdida de velocidad acumulada.
Para float u ≈ 5.96e-8, para double u ≈ 1.11e-16. Teniendo en cuenta que en nuetra simulacion trabajamos con numeros de magnitudes muy pequeñas necesitamos mas precision.  

### Overflow en la aceleracion gravitatoria
Para la aceleración gravitatoria se usa una ecuacion que depende de la distancia al sol al cubo. Esta magnitud al trabajarla en metros puede ser muy grande. Teniendo en cuenta que el numero maximo que se guarda en un float es ~3.4e38, los planetas que se encuntran mas lejanos al sol pueden superar esta barrera. Esto genera que aparezcan Inf/NaN. Con double el margen es enorme, ~1.8e308, y este problema desaparece en la práctica.


## ...Complejidad computacional con asteroides...

Al agregar los 500 asteroides comenzo a correr la simulacion a 15 FPS y rapidamente bajo a 3 FPS. Esto se debe a dos cuellos de botella que habia en le codigo.
1. Física: cálculo de aceleraciones (orbitalSim.cpp)
La simulacion tenia una compljidad O(N²) algoritmica en la aceleracion de los cuerpos, aunque la fuerza se calcule como O(1) por par. Esto se debe a que en cada iteracion en el archivo orbitalSim.cpp en la funcion `updateOrbitalSim(...)` por cada cuerpo se sumaban las contribuciones gravitatorias de todos los demas (N-1 iteraciones por N cuerpos) en un doble bucle.
Con 500 asteroides (más el sistema solar), el número de interacciones por frame ronda los **250k**.
2. Render: Dibujo de los asteroides (view.cpp)
Luego podemos ver que la simulacion tenia un O(N) en el render de los cuerpos, ya que usamos un bucle if de N curpos para su dibujo. El cuello de botella se creo debido a que como la condicion del if era que el radio sea mayor a 0.01f muchos asteroides entraban en esa categoria y usaban la funcion `DrawSphere(...)` que usa muchos mas recursos que `DrawPoint3D(...)`.

Estos dos cullos de botella explican la caída fuerte de FPS al agregar los 500 asteroides.


## ...Mejora de la complejidad computacional...

El primer cambio que hicimos para mejorar la complejidad computacional fue en `updateOrbitalSim(...)`, donde decidimos despreciar el efecto gravitacional que tienen los asteroides sobre el resto de cuerpos esto devido a la gran diferencia de tamaño y peso entre los planetas y el sol y los asteroides lo que hace que al desprecirlo la fuerza gravitatoria de cada cuerpo sea aproximadamente la misma. 
Al considerar este cambio podemos usar la ecuacion: T(N)=P2+(N−P)P=O(N⋅P), donde P son los cuerpos masivos. Bajando la complejidad algoritmica en la aceleracion de los cuerpos a O(N).Luego de este cambio la simulaicion empezo a correr a 15 FPS, precentando una mejora pero aun no llegando a el resultado esperado.
Por ultimo hicimos un cambio en el archivo View.cpp en la funcion `renderView(...)` al dividir a los cuerpos en Asteroides y no asteroides, haciendo un ciclo for para cada uno, utilizando la funion `DrawPoint3D(...)` para todos los asteroides lo que baja la constate del render, aunque se mantenga la comlejidad en O(N). Luego de este cambio la simulaicon correr a 60 FPS (lo esperado).


## ...Bonus points...
Ester egg: Fijar phi en 0 produce un patrón en espiral similar a Fibonacci. 

