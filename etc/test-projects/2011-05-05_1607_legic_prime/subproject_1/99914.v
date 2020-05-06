/** 
 * This is a Verilog implementation for a gate of type 2NOR.
 */

module dg_2NOR (
  a, b, 
  y
);

  // input ports
  input a;
  input b;

  // output ports
  output y;

  assign y = ~(a | b);

endmodule
















