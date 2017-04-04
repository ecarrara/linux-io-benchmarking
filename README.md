Comparação dos Métodos de Copiar de um Arquivo no Linux/Unix
============================================================

Qual é a maneira mais rápida (e que consome menos memória) de se copiar um
arquivo no Linux/Unix?

## Resultados

|                        | System Time (seconds) |
|------------------------|-----------------------|
|     copy_file_range    |       0.26            |
|         splice         |       0.26            |
| simple_read_and_write  |       0.34            |


### Como foi feito

Antes dos testes foi criado um arquivo com 32MB
(`dd if=/dev/urandom of=testfile bs=32M count=1 iflag=fullblock`).

Cada teste foi realizado após um _reboot_ para garantir que não havia dados
do arquivo em cache no sistema operacional.

Foi utilizado o utilitário `time` para executar os diferentes binários gerados
para cada método de cópia.

Se você quiser compilar o código na sua máquina é só rodar `tup upd` na raiz
do projeto (você vai precisar do [Tup](gittup.org) e do GCC).

E depois rodar o `/usr/bin/time` para testar os métodos de cópia. Lembrando de
dar prioridade máximo no uso de IO e CPU para o processo (você vai precisar
ser root pra isso).

```shellscript
ionice -c 1 -n 0 nice -n -20 /usr/bin/time -v ./build/copy_file_range ./testfile ./testfile2 | tee out/copy_file_range.txt
reboot  # :)

ionice -c 1 -n 0 nice -n -20 /usr/bin/time -v ./build/splice ./testfile ./testfile2 | tee out/splice.txt
reboot

ionice -c 1 -n 0 nice -n -20 /usr/bin/time -v ./build/read_and_write ./testfile ./testfile2 | tee out/read_and_write.txt
reboot
```

##### Meu Ambiente de Execução

**Processador:** AMD FX-8320E Eight-Core Processor (e também aquecedor do quarto nas noites frias, AMD...)
**Kernel:** 4.8.13-1 ARCH (ArchLinux)
**Compilador:** gcc 6.3.1 20170306
**Disco:** SanDisk SSD PLUS 240GB (Leituras: 520MB/s, Gravações: 350MB/s)

