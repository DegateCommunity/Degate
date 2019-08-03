/** 
 * This is a Verilog implementation for a gate of type FF2.
 */

module dg_FF2 (
  clk, d, 
  notq, q
);

  // input ports
  input clk;
  input d;

  // output ports
  output notq;
  output q;

  reg q;
  reg notq;

  always @(posedge clk)
    begin
      q <= d;
      notq <= ~q;
    end

endmodule


















