/** 
 * This is a Verilog implementation for a gate of type TRI2.
 */

// XXX not sure if it is a tri1

module dg_TRI2 (
  a, b, 
  y, y2
);

  // input ports
  input a;
  input b;

  // output ports
  output y;
  output y2;

  reg y;

  always @*
    begin
      y = 1'bz; // default
      case({a, b})
        2'b00 : y = 1'bz;
        2'b01 : y = ~a;
        2'b10 : y = 1'bz;
        2'b11 : y = ~a;
      endcase
    end


  assign y2 = ~b;


endmodule
















