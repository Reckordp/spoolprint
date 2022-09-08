require "spoolprint/server/receiver_manager"

module Spoolprint
  class ServerReceipt < SpoolPrint::Server
    ASAL_SERVER_PORT = 7776

    def initialize(spool)
      super(ASAL_SERVER_PORT)
      @spool = spool
      @manager = ReceiverManager.new
      @server_zone = Thread.new { server_bergulir }
    end

    def start
      super
      @server_zone.start
    end

    def stop
      super
      @server_zone.stop
    end

    private
    def server_bergulir
      server_receive while true
    end
    
    def server_receive
      receiver_poll do |r|
        resp = @manager.make_response(r)
        @spool << resp.for_print
        next resp.success
      end
    end
  end
end