require 'spoolprint'
require 'escpos'

module Spoolprint
  class ReceiptPrintEasy < Escpos::Printer
    def intialize(taskName = nil)
      super
      @interface = create_interface(taskName)
    end

    def to_escpos
      @interface.taruh_raw(super)
      return @interface.cetak
    end

    private
    def create_interface(taskName)
      if (taskName.nil?)
        return SpoolPrint.new
      else
        return SpoolPrint.new(taskName)
      end
    end
  end
end