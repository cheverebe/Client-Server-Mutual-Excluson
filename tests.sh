echo "Ejecutando de a 2"
mpiexec -np 2 ./tp3  'a' 500 500 1 500 500 >/dev/null

echo "Ejecutando de a 4"
mpiexec -np 4 ./tp3  'a' 500 500 1 500 500 >/dev/null

echo "Ejecutando de a 20"
mpiexec -np 20 ./tp3  'a' 500 500 1 500 500 >/dev/null

echo "Ejecutando de a 50"
mpiexec -np 50 ./tp3  'a' 500 500 1 500 500 >/dev/null

echo "Diferentes tiempos, 20 procesos"
mpiexec -np 20 ./tp3  'a' 50 100 10 20 10 >/dev/null

echo "Poco tiempo en la seccion critica"
mpiexec -np 20 ./tp3  'a' 10 10 10 20 2 >/dev/null

echo "Con mucho delay en la seccion critica"
mpiexec -np 20 ./tp3  'a' 10 10 5 20 300 >/dev/null

echo "Distinta cantidad de iteraciones"
mpiexec -np 8 ./tp3  'a' 10 10 5 20 300 'b' 10 10 6 20 300 'c' 10 10 7 20 300 'd' 10 10 8 20 300 >/dev/null

echo "Distinto tiempo de computo inicial de cada cliente"
mpiexec -np 8 ./tp3  'a' 10 10 5 20 300 'b' 15 10 5 20 300 'c' 20 10 5 20 300 'd' 25 10 5 20 300 >/dev/null

echo "Distinto tiempo de computo final de cada cliente"
mpiexec -np 8 ./tp3  'a' 10 15 5 20 300 'b' 10 20 5 20 300 'c' 10 25 5 20 300 'd' 10 30 5 20 300 >/dev/null

echo "Distinto tiempo de computo previo a pedir acceso exclusivo (en cada iteracion) de c/cliente"
mpiexec -np 8 ./tp3  'a' 10 10 5 20 300 'b' 10 10 5 25 300 'c' 10 10 5 30 300 'd' 10 10 5 35 300 >/dev/null

echo "Distinto tiempo de computo dentro de la seccion critica (en cada iteracion) de c/cliente."
mpiexec -np 8 ./tp3  'a' 10 10 5 20 300 'b' 10 10 5 20 350 'c' 10 10 5 20 400 'd' 10 10 5 20 450 >/dev/null

echo "Todos los factores distintos"
mpiexec -np 8 ./tp3  'a' 10 25 5 40 300 'b' 15 20 6 35 350 'c' 20 15 7 30 400 'd' 25 10 8 25 450 >/dev/null


