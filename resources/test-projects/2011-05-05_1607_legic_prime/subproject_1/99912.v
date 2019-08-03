/** 
 * This is a Verilog implementation for a gate of type 2NAND.
 */

module dg_2NAND (
  a, b, 
  y
);

  // input ports
  input a;
  input b;

  // output ports
  output y;

  assign y = ~(a & b);

endmodule
















