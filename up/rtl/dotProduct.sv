module dotproduct (
    input  logic         clk_i,
    input  logic         rst_ni,   // Reset ativo baixo
    input  logic         start_i,

    // Activations
    input  logic [31:0]  a0_i, a1_i, a2_i, a3_i, a4_i,
                         a5_i, a6_i, a7_i, a8_i, a9_i,

    // Weights
    input  logic [31:0]  b0_i, b1_i, b2_i, b3_i, b4_i,
                         b5_i, b6_i, b7_i, b8_i, b9_i,

    // Bias
    input  logic [31:0]  c_i,

    output logic [63:0]  result_o,
    output logic         busy_o,
    output logic         result_valid_o
);

    // -------------------------------------------------
    // Estados da FSM
    // -------------------------------------------------
    typedef enum logic [1:0] {
        IDLE,
        COMPUTE,
        DONE
    } state_t;

    state_t state;

    // -------------------------------------------------
    // Registradores internos
    // -------------------------------------------------
    logic signed [63:0] acc;
    logic signed [63:0] result_reg;
    logic        [3:0]  idx;

    logic start_d;
    wire start_pulse;

    // -------------------------------------------------
    // Vetores internos (SIGNED!)
    // -------------------------------------------------
    logic signed [31:0] a [0:9];
    logic signed [31:0] b [0:9];

    // -------------------------------------------------
    // Mapeamento dos inputs
    // -------------------------------------------------
    always_comb begin
        a[0] = a0_i; b[0] = b0_i;
        a[1] = a1_i; b[1] = b1_i;
        a[2] = a2_i; b[2] = b2_i;
        a[3] = a3_i; b[3] = b3_i;
        a[4] = a4_i; b[4] = b4_i;
        a[5] = a5_i; b[5] = b5_i;
        a[6] = a6_i; b[6] = b6_i;
        a[7] = a7_i; b[7] = b7_i;
        a[8] = a8_i; b[8] = b8_i;
        a[9] = a9_i; b[9] = b9_i;
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
    // FSM + Acumulador
    // -------------------------------------------------
    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            state          <= IDLE;
            acc            <= 64'sd0;
            result_reg     <= 64'sd0;
            idx            <= 4'd0;
            busy_o         <= 1'b0;
            result_valid_o <= 1'b0;
        end else begin

            // Reset result_valid_o quando começar novo cálculo
            if (start_pulse) begin
                result_valid_o <= 1'b0;
            end

            case (state)

                // ---------------- IDLE ----------------
                IDLE: begin
                    busy_o <= 1'b0;
                    idx    <= 4'd0;

                    if (start_pulse) begin
                        acc    <= 64'sd0;
                        state  <= COMPUTE;
                        busy_o <= 1'b1;
                    end
                end

                // --------------- COMPUTE ---------------
                COMPUTE: begin
                    busy_o <= 1'b1;

                    // Pruning: só soma se ambos não forem zero
                    if ((a[idx] != 0) && (b[idx] != 0)) begin
                        acc <= acc + ($signed(a[idx]) * $signed(b[idx]));
                    end

                    if (idx == 4'd9) begin
                        // Soma bias e faz shift
                        result_reg <= (acc + $signed(c_i));
                        state <= DONE;
                        result_valid_o <= 1'b1;
                    end else begin
                        idx <= idx + 1;
                    end
                end

                // ---------------- DONE ----------------
                DONE: begin
                    busy_o <= 1'b0;
                    // result_valid_o mantém 1 até próximo start

                    if (start_pulse) begin
                        acc    <= 64'sd0;
                        idx    <= 4'd0;
                        state  <= COMPUTE;
                        busy_o <= 1'b1;
                        result_valid_o <= 1'b0;
                    end
                end

                default: state <= IDLE;

            endcase
        end
    end

    // -------------------------------------------------
    // Saída FINAL
    // -------------------------------------------------
    assign result_o = result_reg;

endmodule
