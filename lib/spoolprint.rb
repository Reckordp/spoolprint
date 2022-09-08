require 'spoolprint.so'
require 'escpos'
require 'spoolprint/server_receipt'

module Spoolprint
  class ReceiptPrintEasy < Escpos::Printer
    attr_reader :interface

    def initialize(job = nil)
      super()
      @interface = create_interface(job)
      @server = ServerReceipt.new(self)
    end

    def start_server
      @server.start
    end

    def stop_server
      @server.stop
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