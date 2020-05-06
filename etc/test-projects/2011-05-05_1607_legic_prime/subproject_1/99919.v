/** 
 * This is a Verilog implementation for a gate of type TRI3.
 */

module dg_TRI3 (
  a, b, c, 
  y
);

  // input ports
  input a;
  input b;
  input c;

  // output ports
  output y;

  reg y;

  always @*
    begin
      y = 1'bz; // default                                                                              
      case({a, b, c})
        3'b000 : y = 1'b1;
        3'b001 : y = ~a; //                                                                             
        3'b010 : y = 1'bz;
        3'b011 : y = 1'bz;
        3'b100 : y = 1'bz;
        3'b101 : y = ~a; // 0                                                                           
        3'b110 : y = 1'bz;
        3'b111 : y = 1'b0;
      endcase
    end

endmodule
















