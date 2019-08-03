/** 
 * This is a Verilog implementation for a gate of type XNOR.
 */

module dg_XNOR (
  a, b, 
  y
);

  // input ports
  input a;
  input b;

  // output ports
  output y;

  assign y = ~(a ^ b);

endmodule


















