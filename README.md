# Informações do projeto
Este projeto conta com um cliente em Java que se comunica com um server em C que utiliza MPI para separação dos processos internos. A organização dos dados é feita a partir de arquivos .txt pré criados junto da pasta onde o programa em C é executado, neste repositório temos algumas pastas que organizam esses arquivos padrões para serem usados. Para a execução normal pode ser usado qualquer uma das pastas, porém para a execução dos testes é recomendado a utilização das pastas organizadas para eles. Além dos arquivos pré criados durante a execução são criados outros arquivos .txt que guardam as informações dos carrinhos e pedidos.

## Teste 1 - Teste de 200 usuários comprando o mesmo produto
Para este teste é necessário a utilização da pasta ```teste1``` onde terá os arquivos .txt junto do executável do programa em C.

## Teste 2 - Teste de 2 clientes comprando aleatoriamente
Para este teste é necessário a utilização da pasta ```teste2``` onde terá os arquivos .txt junto do executável do programa em C.

## Teste 3 - Teste de 10 clientes comprando aleatoriamente
Para este teste é necessário a utilização da pasta ```teste3``` onde terá os arquivos .txt junto do executável do programa em C.

## Teste 4 - Teste de 1000 clientes comprando aleatoriamente
Para este teste é necessário a utilização da pasta ```teste4``` onde terá os arquivos .txt junto do executável do programa em C.

## Pasta de BackupRestore do Estoque
Dentro de todas as pastas de teste existe uma pasta que contem o padrão de entrada para cada teste para facilitar a reconfiguração do teste, basta copiar os arquivos de dentro da pasta de BackupRestoreEstoque e jogar na pasta do teste e substituir sobre os existentes.

# Compilação e execução
Para compilar o cliente Java basta utilizar o javac padrão em suas novas versões. Já o server em C é necessário utilizar o compilador mpicc um exemplo de compilação seria:
``` 
mpicc Server.c -o server
```

Para a execução o cliente Java também é uma execução normal com java em suas novas versões. Já o server em C é necessário executar utilizando o mpirun, e configurar 8 ranks de execução, por utilizar 8 ranks dependendo se sua CPU tem mais ou menos núcleos que a quantidade de rank será necessário passar um parâmetro a mais, exemplos de execução:
- Para CPU com mais de 8 núcleos:
``` 
mpirun -np 8 ./server
```
- Para CPU com menos de 8 núcleos:
``` 
mpirun -np 8 --oversubscribe ./server
```

## Execução dos testes
Para a execução dos testes basta navegar até a pasta do respectivo teste e executar o mpi conforme descrito acima, com o server em execução basta seguir no menu do cliente na opção de teste e escolher a opção do teste respectivo a pasta de execução do server.



# Referências
Para a lógica do socket server em C foi utilizado o seguinte código: https://github.com/halloweeks/networking/blob/main/server.cpp