Aluno: Anderson Kmetiuk

![img](https://github.com/andersonkmetiuk/ELF74-2022-1/blob/main/Lab6/Escalonamento.png)

Os tempos foram calculados utilizando um loop de um tempo considerável e um cronômetro, com exceção do caso das questões "b" e "c" onde foi utilizada a função tx_time_get();
Nessas questões foi difícil a visualização para medir com o cronômetro, por esse motivo foi utilizada essa função

a) Nesse primeiro caso, todas as tarefas tem a mesma prioridade, não têm preempção, o time slice é fixo de 50ms. Então todas as threads rodam em paralelo

b) Como não temos preempção e as tarefas não têm time slice, elas permanecem na mesma tarefa até finalizá-la. Nessa execução levamos em conta a prioridade, mas como a prioridade está na ordem correta não há inversão de prioridade.

c) Nesse caso temos o escalonamento por prioridade, ou seja, as threads só serão interrompidas por threads com maior prioridade, como no caso as prioridades estão na ordem correta, nenhum thread interrompe a outra.

d) Com a utilização do Mutex nas Threads 1 e 3 podemos notar que não há preempção entre essas threads. Como não há herança de prioridade caso a thread 1 esteja bloqueada, a thread 2 não consegue executar e precisa esperar.

e) Com a herança de prioridade, a thread 2 pode ganhar prioridade pra que a thread 3 execute posteriormente