from migen import *
from litex.gen import *
from litex.soc.interconnect.csr import *

class DotProduct(LiteXModule):
    def __init__(self, platform):

        # importa bloco em hardware
        platform.add_source("rtl/dotProduct.sv")

        # CSRs
        self._start = CSRStorage(fields=[
            CSRField("start", size=1, offset=0, pulse=True),
        ])
        
        #self._n = CSRStorage(32, name="n")
        
        #self._a = [CSRStorage(32, name=f"a{i}") for i in range(8)]
        #self._b = [CSRStorage(32, name=f"b{i}") for i in range(8)]
        
        self._a0 = CSRStorage(32, name="a0")
        self._a1 = CSRStorage(32, name="a1")
        self._a2 = CSRStorage(32, name="a2")
        self._a3 = CSRStorage(32, name="a3")
        self._a4 = CSRStorage(32, name="a4")
        self._a5 = CSRStorage(32, name="a5")
        self._a6 = CSRStorage(32, name="a6")
        self._a7 = CSRStorage(32, name="a7")
        self._a8 = CSRStorage(32, name="a8")
        self._a9 = CSRStorage(32, name="a9")
        
        self._b0 = CSRStorage(32, name="b0")
        self._b1 = CSRStorage(32, name="b1")
        self._b2 = CSRStorage(32, name="b2")
        self._b3 = CSRStorage(32, name="b3")
        self._b4 = CSRStorage(32, name="b4")
        self._b5 = CSRStorage(32, name="b5")
        self._b6 = CSRStorage(32, name="b6")
        self._b7 = CSRStorage(32, name="b7")
        self._b8 = CSRStorage(32, name="b8")
        self._b9 = CSRStorage(32, name="b9")
        
        self._c = CSRStorage(32, name="bias")
        
        self._nnz = CSRStorage(8, name="nnz")
        self._result = CSRStatus(64, name="result")
        self._busy   = CSRStatus(1, name="busy")
        self._cycles = CSRStatus(32, name="cycles")


        # sinais internos
        start_sig  = Signal()
        
        a_sigs      =  [Signal((32, True)) for _ in range(10)]
        b_sigs      =  [Signal((32, True)) for _ in range(10)]
        
        c_sig = Signal((32, True))
        
        nnz_sig = Signal(8)
        
        result_sig = Signal((64, True))
        busy_sig   = Signal()
        
        cycles_sig = Signal(32)

        # instância do módulo SV
        self.specials += Instance("dotproduct",
            i_clk_i    = ClockSignal(),
            i_rst_ni   = ~ResetSignal(),
            i_start_i  = start_sig,
            i_nnz_i    = nnz_sig,
            # Vetores A
            **{f"i_a{i}_i": a_sigs[i] for i in range(10)},
            # Vetores B
            **{f"i_b{i}_i": b_sigs[i] for i in range(10)},
            
            i_c_i = c_sig,
            
            o_result_o = result_sig,
            o_busy_o   = busy_sig,
            o_cycle_count_o = cycles_sig
        )

        # Mapeamento dos CSRs -> sinais internos
        self.comb += [
            start_sig.eq(self._start.fields.start),
            self._result.status.eq(result_sig),
            self._busy.status.eq(busy_sig),
            
            c_sig.eq(self._c.storage), # bias
            nnz_sig.eq(self._nnz.storage),
            self._cycles.status.eq(cycles_sig),

            a_sigs[0].eq(self._a0.storage),
            a_sigs[1].eq(self._a1.storage),
            a_sigs[2].eq(self._a2.storage),
            a_sigs[3].eq(self._a3.storage),
            a_sigs[4].eq(self._a4.storage),
            a_sigs[5].eq(self._a5.storage),
            a_sigs[6].eq(self._a6.storage),
            a_sigs[7].eq(self._a7.storage),
            a_sigs[8].eq(self._a8.storage),
            a_sigs[9].eq(self._a9.storage),

            b_sigs[0].eq(self._b0.storage),
            b_sigs[1].eq(self._b1.storage),
            b_sigs[2].eq(self._b2.storage),
            b_sigs[3].eq(self._b3.storage),
            b_sigs[4].eq(self._b4.storage),
            b_sigs[5].eq(self._b5.storage),
            b_sigs[6].eq(self._b6.storage),
            b_sigs[7].eq(self._b7.storage),
            b_sigs[8].eq(self._b8.storage),
            b_sigs[9].eq(self._b9.storage),
        ]
        
