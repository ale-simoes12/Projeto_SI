import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.Random;

public class Client {
    public static void main(String args[]) throws Exception {
        Scanner menu = new Scanner (System.in);
        String usuario = "aaaaaaaaaaaaaaaaa";

        while (usuario.length() > 10) {
            System.out.println("Digite o nome do usuario (Máximo de 10 caracteres):");
            usuario = menu.nextLine();
            if(usuario.length() > 10) {
                System.out.println("Nome de usuário maior que o permitido digite novamente!");
            } else if(usuario.length() < 10) {
                int tam = usuario.length();
                for (int i = 0; i < 10 - tam; i++) {
                    usuario += ".";
                }
            }
        }

        while (true) {            

            System.out.print("##------------Menu------------##\n\n");
            System.out.print("|------------------------------|\n");
            System.out.print("| Opção 1 - Vizualisar Produtos|\n");
            System.out.print("| Opção 2 - Comprar Carrinho   |\n");
            System.out.print("| Opção 3 - Mostrar Pedidos    |\n");
            System.out.print("| Opção 4 - Testes             |\n");
            System.out.print("| Opção 5 - Sair               |\n");
            System.out.print("|------------------------------|\n");
            System.out.print("Digite uma opção: ");

            int opcao = menu.nextInt();

            if (opcao == 5) {
                System.out.print("\nAté logo!");
                menu.close(); 
            } else {

                switch (opcao) {
                case 1:
                    VizualisarProdutos(menu, usuario);
                    break;

                case 2:
                    PagarCarrinho(menu, usuario);
                    break;

                case 3:
                    VerPedidos(menu, usuario);
                    break;

                case 4:
                    Testes(menu, usuario);
                    break;

                default:
                    System.out.println("\nOpção Inválida!");
                    break;
                }
            }
        }
    }

    private static void VizualisarProdutos(Scanner menu, String usuario) {
        while (true) {            

            String resp;
            String itens[];
            String dados[];
            int cont = 0;
            Cliente Cliente = new Cliente("192.168.15.4", 8080, "veritem");
            resp = Cliente.getResp();
            itens = resp.split(";");
            System.out.println("Produtos disponíveis:");
            for (String i: itens){
                dados = i.split(",");
                System.out.println(cont + ": " + dados[0]);
                cont++;
            }
            System.out.println(cont + ": Voltar ao menu");
            System.out.println("Selecione uma opção:");
            
            int opcao = menu.nextInt();

            if (opcao == cont) {
                return;
            } else if (opcao < cont) {
                System.out.println("Detalhes do produto:");
                dados = itens[opcao].split(",");
                System.out.println("Nome: " + dados[0] + "\nValor: " + dados[1] + "\nEstoque: " + dados[2]);

                System.out.println("Deseja comprar o produto?");
                System.out.println("1: Sim");
                System.out.println("2: Não");

                int opcao2 = menu.nextInt();
                
                switch (opcao2) {
                    case 1:
                        System.out.println("Digite a quantidade que deseja colocar no carrinho: ");
                        int qtd = menu.nextInt();
                        String qtdS = "";
                        if (qtd >= 100) {
                            qtdS = "" + qtd;
                        } else if (qtd >= 10 && qtd < 100) {
                            qtdS = "0" + qtd;
                        } else if (qtd >= 0 && qtd < 10) {
                            qtdS = "00" + qtd;
                        }
                        String req = "addcar" + usuario + dados[0] + qtdS;
                        Cliente Cliente2 = new Cliente("192.168.15.4", 8080, req);
                        resp = Cliente2.getResp();
                        System.out.println(resp);
                        break;
        
                    case 2:
                        break;
        
                    default:
                        System.out.println("\nOpção Inválida!");
                        break;
                }
            } else {
                System.out.println("Opção invalida!");
            }
        }
    }

    private static void PagarCarrinho(Scanner menu, String usuario) {
        
        String resp;
        String carrinho[];
        String itens[];
        String dados[];
        Cliente Cliente = new Cliente("192.168.15.4", 8080, "vercarrinho"+usuario);
        resp = Cliente.getResp();

        if (!resp.contains("Carrinho não existe")){
            carrinho = resp.split(":");
            System.out.println("Carrinho de " + carrinho[0] + ": ");
            itens = carrinho[1].split(";");
            for (String i: itens){
                dados = i.split(",");
                System.out.println("Nome do produto: " + dados[0] + "\tQuantidade: " + dados[1]);
            }
            System.out.println("Deseja pagar o carrinho?");
            System.out.println("1: Sim");
            System.out.println("2: Não");

            int opcao2 = menu.nextInt();
            
            switch (opcao2) {
                case 1:
                    for (String i: itens){
                        dados = i.split(",");
                        String req = "compra" + dados[0] + dados[1];
                        Cliente Cliente2 = new Cliente("192.168.15.4", 8080, req);
                        resp = Cliente2.getResp();
                        System.out.println(resp);
                    }
                    String req2 = "criapedido" + usuario;
                    Cliente Cliente3 = new Cliente("192.168.15.4", 8080, req2);
                    resp = Cliente3.getResp();
                    System.out.println(resp);
                    break;

                case 2:
                    break;

                default:
                    System.out.println("\nOpção Inválida!");
                    break;
            }
        } else {
            System.out.println(resp);
        }
        
    }

    private static void VerPedidos(Scanner menu, String usuario) {

        String resp;
        String pedidos[];
        String num[];
        String itens[];
        String dados[];
        Cliente Cliente = new Cliente("192.168.15.4", 8080, "verpedidos");
        resp = Cliente.getResp();
        pedidos = resp.split(";");
        for (String i: pedidos){
            num = i.split(":");
            itens = num[1].split("-");
            System.out.println("Pedido " + num[0] + " do usuario " + itens[0].split(",")[0] + ": ");
            for (String j: itens){
                dados = j.split(",");
                System.out.println("\tNome do produto: " + dados[1] + "\tQuantidade: " + dados[2]);
            }
        }
        
        System.out.println("1: Ver estoque dos produtos");
        System.out.println("2: Voltar ao menu");

        int opcao = menu.nextInt();
            
        switch (opcao) {
            case 1:
                Cliente Cliente2 = new Cliente("192.168.15.4", 8080, "veritem");
                resp = Cliente2.getResp();
                itens = resp.split(";");
                System.out.println("Produtos :");
                for (String i: itens){
                    dados = i.split(",");
                    System.out.println("Nome : " + dados[0] + "\tEstoque: " + dados[2]);
                }
                System.out.println("1: Voltar");
                System.out.println("Selecione uma opção:");

                int opcao2 = menu.nextInt();
                    
                switch (opcao2) {
                    case 1:
                        break;

                    default:
                        System.out.println("\nOpção Inválida!");
                        return;
                }

            case 2:
                return;

            default:
                System.out.println("\nOpção Inválida!");
                return;
        }
    }
        
    private static void Testes(Scanner menu, String usuario) {
        while (true) {
            System.out.print("##-------------------------Testes-------------------------##\n\n");
            System.out.print("|----------------------------------------------------------|\n");
            System.out.print("| Leia o README para configurar os arquivos para os testes |\n");
            System.out.print("|----------------------------------------------------------|\n");
            System.out.print("| Opção 1 - Teste de 200 usuários comprando o mesmo produto|\n");
            System.out.print("| Opção 2 - Teste de 2 clientes comprando aleatoriamente   |\n");
            System.out.print("| Opção 3 - Teste de 10 clientes comprando aleatoriamente  |\n");
            System.out.print("| Opção 4 - Teste de 1000 clientes comprando aleatoriamente|\n");
            System.out.print("| Opção 5 - Sair                                           |\n");
            System.out.print("|----------------------------------------------------------|\n");
            System.out.print("Digite uma opção: ");

            int opcao = menu.nextInt();

            if (opcao == 5) {
                return;
            } else {

                switch (opcao) {
                case 1:
                    Teste1(menu, usuario);
                    break;

                case 2:
                    Teste2(menu, usuario);
                    break;

                case 3:
                    Teste3(menu, usuario);
                    break;

                case 4:
                    Teste4(menu, usuario);
                    break;


                default:
                    System.out.println("\nOpção Inválida!");
                    break;
                }
            }
        }
    }
    
    private static void Teste1(Scanner menu, String usuario) {
        List<Thread> threads = new ArrayList<Thread>();
        for(int i = 0;i < 200; i++) {
            String usu = "";
            if (i < 10) {
                usu = String.valueOf(i)+".........";
            } else if (i >= 10 && i < 100) {
                usu = String.valueOf(i)+"........";
            } else if (i >= 100 && i < 1000) {
                usu = String.valueOf(i)+".......";
            } else if (i >= 1000 && i < 10000) {
                usu = String.valueOf(i)+"......";
            }
            Teste1thread thread = new Teste1thread(usu);
            Thread exec = new Thread(thread);
            threads.add(exec);
            exec.start();
        }
        
        for(Thread t: threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    private static void Teste2(Scanner menu, String usuario) {
        List<Thread> threads = new ArrayList<Thread>();
        for(int i = 0;i < 2; i++) {
            String usu = "";
            if (i < 10) {
                usu = String.valueOf(i)+".........";
            } else if (i >= 10 && i < 100) {
                usu = String.valueOf(i)+"........";
            } else if (i >= 100 && i < 1000) {
                usu = String.valueOf(i)+".......";
            } else if (i >= 1000 && i < 10000) {
                usu = String.valueOf(i)+"......";
            }
            Teste2thread thread = new Teste2thread(usu);
            Thread exec = new Thread(thread);
            threads.add(exec);
            exec.start();
        }
        
        for(Thread t: threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    private static void Teste3(Scanner menu, String usuario) {
        List<Thread> threads = new ArrayList<Thread>();
        for(int i = 0;i < 10; i++) {
            String usu = "";
            if (i < 10) {
                usu = String.valueOf(i)+".........";
            } else if (i >= 10 && i < 100) {
                usu = String.valueOf(i)+"........";
            } else if (i >= 100 && i < 1000) {
                usu = String.valueOf(i)+".......";
            } else if (i >= 1000 && i < 10000) {
                usu = String.valueOf(i)+"......";
            }
            Teste3thread thread = new Teste3thread(usu);
            Thread exec = new Thread(thread);
            threads.add(exec);
            exec.start();
        }
        
        for(Thread t: threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    private static void Teste4(Scanner menu, String usuario) {
        List<Thread> threads = new ArrayList<Thread>();
        for(int i = 0;i < 1000; i++) {
            String usu = "";
            if (i < 10) {
                usu = String.valueOf(i)+".........";
            } else if (i >= 10 && i < 100) {
                usu = String.valueOf(i)+"........";
            } else if (i >= 100 && i < 1000) {
                usu = String.valueOf(i)+".......";
            } else if (i >= 1000 && i < 10000) {
                usu = String.valueOf(i)+"......";
            }
            Teste4thread thread = new Teste4thread(usu);
            Thread exec = new Thread(thread);
            threads.add(exec);
            exec.start();
        }
        
        for(Thread t: threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

class Teste1thread implements Runnable {

    Socket Cliente;
    String usuario;

    Teste1thread(String usuario){
        this.usuario = usuario;
    }

    public void run() {
        System.out.println("Iniciando "+usuario);
        Cliente Cliente = new Cliente("192.168.15.4", 8080, "addcar"+usuario+"Celular...0001");
        String resp = Cliente.getResp();
        if (resp.contains("Item adicionado ao carrinho!")) {
            Cliente Cliente2 = new Cliente("192.168.15.4", 8080, "criapedido"+usuario);
            Cliente Cliente3 = new Cliente("192.168.15.4", 8080, "compraCelular...0001");
            String resp3 = Cliente3.getResp();
            String resp2 = Cliente2.getResp();
            String print = resp2;
            if (resp3.contains("sem estoque disponível!")) {
                print = resp3;
            }
            System.out.println("Usuario "+usuario+": "+print);
        } else {
            System.out.println("Usuario "+usuario+": Erro ao adicionar ao carrinho!");
        }
    }
}

class Teste2thread implements Runnable {

    Socket Cliente;
    String usuario;

    Teste2thread(String usuario){
        this.usuario = usuario;
    }

    public void run() {
        System.out.println("Iniciando "+usuario);
        Random gerador = new Random();
        int qtdProd = gerador.nextInt(3) + 2;
        String prodList[] = {"Celular...","Mouse.....","Teclado...","Fone......","Relogio..."};
        List<String> prod = new ArrayList<String>();
        for (int i = 0; i < qtdProd; i++) {
            int qualProd = gerador.nextInt(5);
            prod.add(prodList[qualProd]);
        }
        for (String produto: prod) {
            Cliente Cliente = new Cliente("192.168.15.4", 8080, "addcar"+usuario+produto);
            String resp = Cliente.getResp();
            if (resp.contains("Item adicionado ao carrinho!")) {
                Cliente Cliente2 = new Cliente("192.168.15.4", 8080, "criapedido"+usuario);
                Cliente Cliente3 = new Cliente("192.168.15.4", 8080, "compra"+produto+"0001");
                String resp3 = Cliente3.getResp();
                String resp2 = Cliente2.getResp();
                String print = resp2;
                if (resp3.contains("sem estoque disponível!")) {
                    print = resp3;
                }
                System.out.println("Usuario "+usuario+": "+print+"\tItem: "+produto);
            } else {
                System.out.println("Usuario "+usuario+": Erro ao adicionar ao carrinho!");
            }
        }
    }
}

class Teste3thread implements Runnable {

    Socket Cliente;
    String usuario;

    Teste3thread(String usuario){
        this.usuario = usuario;
    }

    public void run() {
        System.out.println("Iniciando "+usuario);
        Random gerador = new Random();
        int qtdProd = gerador.nextInt(3) + 2;
        String prodList[] = {"Celular...","Mouse.....","Teclado...","Fone......","Relogio...","Bone......","Fone......","Meia......","Oculos....","Tenis....."};
        List<String> prod = new ArrayList<String>();
        for (int i = 0; i < qtdProd; i++) {
            int qualProd = gerador.nextInt(10);
            prod.add(prodList[qualProd]);
        }
        for (String produto: prod) {
            Cliente Cliente = new Cliente("192.168.15.4", 8080, "addcar"+usuario+produto);
            String resp = Cliente.getResp();
            if (resp.contains("Item adicionado ao carrinho!")) {
                Cliente Cliente2 = new Cliente("192.168.15.4", 8080, "criapedido"+usuario);
                Cliente Cliente3 = new Cliente("192.168.15.4", 8080, "compra"+produto+"0001");
                String resp3 = Cliente3.getResp();
                String resp2 = Cliente2.getResp();
                String print = resp2;
                if (resp3.contains("sem estoque disponível!")) {
                    print = resp3;
                }
                System.out.println("Usuario "+usuario+": "+print+"\tItem: "+produto);
            } else {
                System.out.println("Usuario "+usuario+": Erro ao adicionar ao carrinho!");
            }
        }
    }
}

class Teste4thread implements Runnable {

    Socket Cliente;
    String usuario;

    Teste4thread(String usuario){
        this.usuario = usuario;
    }

    public void run() {
        System.out.println("Iniciando "+usuario);
        Random gerador = new Random();
        int qtdProd = 1;
        String prodList[] = {"Celular...","Mouse.....","Teclado...","Fone......","Relogio...","Bone......","Fone......","Meia......","Oculos....","Tenis....."};
        List<String> prod = new ArrayList<String>();
        for (int i = 0; i < qtdProd; i++) {
            int qualProd = gerador.nextInt(10);
            prod.add(prodList[qualProd]);
        }
        for (String produto: prod) {
            Cliente Cliente = new Cliente("192.168.15.4", 8080, "addcar"+usuario+produto);
            String resp = Cliente.getResp();
            if (resp.contains("Item adicionado ao carrinho!")) {
                Cliente Cliente2 = new Cliente("192.168.15.4", 8080, "criapedido"+usuario);
                Cliente Cliente3 = new Cliente("192.168.15.4", 8080, "compra"+produto+"0001");
                String resp3 = Cliente3.getResp();
                String resp2 = Cliente2.getResp();
                String print = resp2;
                if (resp3.contains("sem estoque disponível!")) {
                    print = resp3;
                }
                System.out.println("Usuario "+usuario+": "+print+"\tItem: "+produto);
            } else {
                System.out.println("Usuario "+usuario+": Erro ao adicionar ao carrinho!");
            }
        }
    }
}

class Cliente {
	// initialize socket and input output streams
	private Socket socket = null;
    private BufferedReader input = null;
    private PrintStream out = null;
    public DataInputStream stream;
    private String fromCliente;
    private String resp = "";

	// constructor to put ip address and port
	public Cliente(String address, int port, String fromCliente)
	{
		// establish a connection
		try {
			socket = new Socket(address, port);
			// System.out.println("Connected");

			// takes input from terminal
            stream = new DataInputStream(socket.getInputStream());
            input = new BufferedReader( new InputStreamReader (socket.getInputStream()));

			// sends output to the socket
            out = new PrintStream(socket.getOutputStream());
		}
		catch (UnknownHostException u) {
			System.out.println(u);
			return;
		}
		catch (IOException i) {
			System.out.println(i);
			return;
		}

		// string to read message from input
		// String envia = "Oi";

        out.println(fromCliente); // Envia para o server C
        
        try {
            // b = stream.readByte();
            resp = input.readLine();
            // System.out.println("Recebido:");
            // System.out.println((char)b);
            // System.out.println(resp);
            // System.out.println(line);
        } catch (IOException e) {
            e.printStackTrace();
        }

		// close the connection
		try {
			input.close();
			out.close();
			socket.close();
		}
		catch (IOException i) {
			System.out.println(i);
		}
	}

    public String getResp() {
        return this.resp;
    }
}
