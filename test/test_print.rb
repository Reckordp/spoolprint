require 'spoolprint'

receipt = Spoolprint::ReceiptPrintEasy.new
receipt << "Z" + Escpos.sequence(Escpos::CTL_LF)
receipt.to_escpos