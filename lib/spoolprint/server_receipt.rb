require "spoolprint/server/receiver_manager"

module Spoolprint
  class ServerReceipt < Server
    ASAL_SERVER_PORT = 7776

    def initialize(spool)
      super(ASAL_SERVER_PORT)
      @spool = spool
      @manager = ReceiverManager.new(method(:read_driver))
      @server_zone = Thread.new { server_bergulir }
      @bergulir = false
    end

    def start
      @bergulir = true
      @server_zone.run if @server_zone.stop?
    end

    def stop
      @bergulir = false
    end

    private
    def server_bergulir
      while true
        @bergulir ? server_receive : Thread.stop?
      end
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