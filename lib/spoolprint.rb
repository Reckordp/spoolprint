require 'spoolprint.so'
require 'escpos'

module Spoolprint
  class ReceiptPrintEasy < Escpos::Printer
    def initialize(job = nil)
      super
      @interface = create_interface(job)
    end

    def to_escpos
      @interface.taruh_raw(super)
      return @interface.cetak
    end

    private
    def create_interface(job)
      if (job.nil?)
        return SpoolPrint.new
      else
        return SpoolPrint.new(job)
      end
    end
  end
end