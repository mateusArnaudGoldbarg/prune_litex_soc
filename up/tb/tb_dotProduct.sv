`timescale 1ns/1ns // Define a resolução de tempo

module dotproduct_tb;

    // -----------------------------------------------------------------
    // Sinais para Conexão com o DUT (Device Under Test)
    // -----------------------------------------------------------------
    logic clk_i;
    logic rst_ni; // Reset Ativo Baixo
    logic start_i;
    
    // Vetores de Entrada (Dados de 8 elementos de 32 bits)
    logic [31:0] a0_i, a1_i, a2_i, a3_i, a4_i, a5_i, a6_i, a7_i;
    logic [31:0] b0_i, b1_i, b2_i, b3_i, b4_i, b5_i, b6_i, b7_i;

    // Saídas
    logic [63:0] result_o;
    logic busy_o;
    
    // Variáveis para a Verificação (Modelo de Referência)
    longint unsigned expected_result;
    
    // -----------------------------------------------------------------
    // Instanciação do DUT
    // -----------------------------------------------------------------
    dotproduct DUT (
        .clk_i(clk_i),
        .rst_ni(rst_ni),
        .start_i(start_i),
        .a0_i(a0_i), .a1_i(a1_i), .a2_i(a2_i), .a3_i(a3_i),
        .a4_i(a4_i), .a5_i(a5_i), .a6_i(a6_i), .a7_i(a7_i),
        .b0_i(b0_i), .b1_i(b1_i), .b2_i(b2_i), .b3_i(b3_i),
        .b4_i(b4_i), .b5_i(b5_i), .b6_i(b6_i), .b7_i(b7_i),
        .result_o(result_o),
        .busy_o(busy_o)
    );

    // -----------------------------------------------------------------
    // Geração do Clock (50% duty cycle, período de 10ns)
    // -----------------------------------------------------------------
    initial begin
        clk_i = 1'b0;
        forever #5 clk_i = ~clk_i; // Clock de 100MHz (período de 10ns)
    end

    // -----------------------------------------------------------------
    // Sequência de Teste Principal
    // -----------------------------------------------------------------
    initial begin
        // Valores de Teste (Exemplo: (1*1) + (2*2) + ... + (8*8) = 204)
        a0_i = 32'd1; b0_i = 32'd1; 
        a1_i = 32'd2; b1_i = 32'd2;
        a2_i = 32'd3; b2_i = 32'd3; 
        a3_i = 32'd4; b3_i = 32'd4;
        a4_i = 32'd5; b4_i = 32'd5; 
        a5_i = 32'd6; b5_i = 32'd6;
        a6_i = 32'd7; b6_i = 32'd7; 
        a7_i = 32'd8; b7_i = 32'd8;

        // 1. Cálculo do Resultado Esperado (Modelo de Referência)
        expected_result = (a0_i * b0_i) + (a1_i * b1_i) + (a2_i * b2_i) + (a3_i * b3_i) + 
                          (a4_i * b4_i) + (a5_i * b5_i) + (a6_i * b6_i) + (a7_i * b7_i);
        
        $display("--- Teste de Produto Escalar ---");
        $display("Resultado Esperado: %0d (0x%h)", expected_result, expected_result);

        // 2. Reset Inicial
        start_i = 1'b0;
        rst_ni  = 1'b0; // Reset Ativo Baixo
        #20; // Espera 20ns (2 ciclos de clock) no reset
        rst_ni  = 1'b1; // Tira o reset
        #5; // Sincroniza com a borda de subida do clock

        // 3. Pulso 'start' (Detecção de borda no próximo ciclo)
        @(posedge clk_i) start_i = 1'b1;
        @(posedge clk_i) start_i = 1'b0; // Start de 1 ciclo

        // 4. Aguarda a Conclusão do Cálculo
        // O cálculo leva 8 ciclos (de idx=0 até idx=7) + 1 ciclo para o resultado final
        // Total: 9 ciclos após o pulso de start
        $display("Início do cálculo. Busy_o: %0b", busy_o);
        
        // Aguarda busy_o voltar a zero (indicando fim)
        while (busy_o) begin
            @(posedge clk_i);
        end
        
        // Espera mais um ciclo para o result_o ser estabilizado 
        // (O seu código já faz isso: result_o é atualizado no mesmo ciclo que busy_reg vira 0)
        @(posedge clk_i); 
        
        $display("Fim do cálculo. Busy_o: %0b", busy_o);
        $display("Resultado Obtido: %0d (0x%h)", result_o, result_o);
        
        // 5. Verificação (Self-Checking)
        if (result_o === expected_result) begin
            $display(">>> TESTE BEM SUCEDIDO: O resultado coincide.");
        end else begin
            $error(">>> TESTE FALHOU! O resultado obtido não coincide com o esperado.");
        end

        // 6. Encerra a Simulação
        #100 $finish; 
    end

endmodule