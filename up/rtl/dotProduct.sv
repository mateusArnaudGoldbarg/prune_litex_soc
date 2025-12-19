module dotproduct (
    input  logic         clk_i,
    input  logic         rst_ni,   // Reset ativo baixo
    input  logic         start_i,

    // Número de elementos NÃO-ZERO
    input  logic [7:0]   nnz_i,

    // Activations (compactadas)
    input  logic [31:0]  a0_i, a1_i, a2_i, a3_i, a4_i,
                         a5_i, a6_i, a7_i, a8_i, a9_i,

    // Weights (compactados)
    input  logic [31:0]  b0_i, b1_i, b2_i, b3_i, b4_i,
                         b5_i, b6_i, b7_i, b8_i, b9_i,

    // Bias
    input  logic [31:0]  c_i,

    // Saídas
    output logic [63:0]  result_o,
    output logic         busy_o,
    output logic         result_valid_o,

    // Contador de ciclos de COMPUTE
    output logic [31:0]  cycle_count_o
);

    // -------------------------------------------------
    // FSM
    // -------------------------------------------------
    typedef enum logic [1:0] {
        IDLE,
        COMPUTE,
        DONE
    } state_t;

    state_t state;

    // -------------------------------------------------
    // Registradores
    // -------------------------------------------------
    logic signed [63:0] acc;
    logic signed [63:0] result_reg;
    logic        [3:0]  idx;

    logic [31:0] cycle_cnt;
    logic [31:0] cycle_cnt_reg;

    logic start_d;
    wire start_pulse;

    // -------------------------------------------------
    // Vetores internos
    // -------------------------------------------------
    logic signed [31:0] a [0:9];
    logic signed [31:0] b [0:9];

    // -------------------------------------------------
    // Mapeamento dos inputs
    // -------------------------------------------------
    always_comb begin
        a[0]=a0_i; b[0]=b0_i;
        a[1]=a1_i; b[1]=b1_i;
        a[2]=a2_i; b[2]=b2_i;
        a[3]=a3_i; b[3]=b3_i;
        a[4]=a4_i; b[4]=b4_i;
        a[5]=a5_i; b[5]=b5_i;
        a[6]=a6_i; b[6]=b6_i;
        a[7]=a7_i; b[7]=b7_i;
        a[8]=a8_i; b[8]=b8_i;
        a[9]=a9_i; b[9]=b9_i;
    end

    // -------------------------------------------------
    // Detecção de pulso de start
    // -------------------------------------------------
    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni)
            start_d <= 1'b0;
        else
            start_d <= start_i;
    end

    assign start_pulse = start_i & ~start_d;

    // -------------------------------------------------
    // FSM + COMPUTE + CONTADOR DE CICLOS
    // -------------------------------------------------
    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            state          <= IDLE;
            acc            <= 64'sd0;
            result_reg     <= 64'sd0;
            idx            <= 4'd0;
            busy_o         <= 1'b0;
            result_valid_o <= 1'b0;
            cycle_cnt      <= 32'd0;
            cycle_cnt_reg  <= 32'd0;
        end else begin

            if (start_pulse)
                result_valid_o <= 1'b0;

            case (state)

                // ---------------- IDLE ----------------
                IDLE: begin
                    busy_o    <= 1'b0;
                    idx       <= 4'd0;
                    cycle_cnt <= 32'd0;

                    if (start_pulse && nnz_i != 0) begin
                        acc    <= 64'sd0;
                        state  <= COMPUTE;
                        busy_o <= 1'b1;
                    end
                end

                // --------------- COMPUTE ---------------
                COMPUTE: begin
                    busy_o <= 1'b1;

                    // 1 MAC por ciclo
                    acc <= acc + ($signed(a[idx]) * $signed(b[idx]));
                    cycle_cnt <= cycle_cnt + 1;

                    if (idx == nnz_i - 1) begin
                        result_reg     <= acc + $signed(c_i);
                        cycle_cnt_reg  <= cycle_cnt + 1; // inclui este ciclo
                        state          <= DONE;
                        result_valid_o <= 1'b1;
                    end else begin
                        idx <= idx + 1;
                    end
                end

                // ---------------- DONE ----------------
                DONE: begin
                    busy_o <= 1'b0;

                    if (start_pulse && nnz_i != 0) begin
                        acc            <= 64'sd0;
                        idx            <= 4'd0;
                        cycle_cnt      <= 32'd0;
                        state          <= COMPUTE;
                        busy_o         <= 1'b1;
                        result_valid_o <= 1'b0;
                    end
                end

            endcase
        end
    end

    // -------------------------------------------------
    // Saídas
    // -------------------------------------------------
    assign result_o       = result_reg;
    assign cycle_count_o  = cycle_cnt_reg;

endmodule

