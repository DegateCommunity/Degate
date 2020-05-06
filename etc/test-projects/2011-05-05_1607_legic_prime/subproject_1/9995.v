/** 
 * This is a Verilog implementation for a gate of type 2AOI.
 */

module dg_2AOI (
  a, b, c, d, 
  y
);

  // input ports
  input a;
  input b;
  input c;
  input d;

  // output ports
  output y;

`ifdef IMPL_WITH_TRANS

   wire   w1, w2, w3, w4;

   supply1 vdd; // predefined high potential                                                            
   supply0 gnd; // predefined potential for ground                                                      

   pmos p1(w1, vdd, b);   // (drain, source, gate)                                                      
   pmos p2(y, w1, c);   // (drain, source, gate)                                                        

   pmos p3(y, w2, a);   // (drain, source, gate)                                                        
   pmos p4(w2, vdd, d);



   nmos n1(w3, gnd, b);   // (drain, source, gate)                                                      
   nmos n2(y, w3, a);   // (drain, source, gate)                                                        

   nmos n3(y, w4, c);
   nmos n4(w4, gnd, d);

`else // !`ifdef IMPL_WITH_TRANS
   reg     y;

   always @*
     begin
        casex({a, b, c, d})
          4'b0000 : y = 1'b1;
          4'b0001 : y = 1'b1;
          4'b000x : y = 1'b1;

          4'b0010 : y = 1'b1;
          4'b0011 : y = 1'b0;

          4'b0100 : y = 1'b1;
          4'b0101 : y = 1'bz;

          4'b0110 : y = 1'b1;
          4'b0111 : y = 1'b0;

          4'b1000 : y = 1'b1;
          4'b1001 : y = 1'b1;
          4'b100x : y = 1'b1;

          4'b1010 : y = 1'bz;

          4'b1011 : y = 1'b0;
          4'b1100 : y = 1'b0;
          4'b110x : y = 1'b0;

          4'b1110 : y = 1'b0;
          4'b1111 : y = 1'b0;
          4'b111x : y = 1'b0;

          default: y = 1'b0;
        endcase
     end

`endif // !`ifdef IMPL_WITH_TRANS 

endmodule


















