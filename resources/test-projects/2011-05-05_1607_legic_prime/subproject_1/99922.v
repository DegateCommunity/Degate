/** 
 * This is a Verilog implementation for a gate of type INV1.
 */

module dg_INV1 (
  a, 
  y
);

  // input ports
  input a;

  // output ports
  output y;

  assign y = ~a;

endmodule
















