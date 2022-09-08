module Spoolprint
  class ReceiverManager
    FLAG_SUCCESS = [0x80, 0x00].collect(&:chr).join

    class RemoteServer
      def initialize(meth)
        @meth = meth
      end

      def use(length)
        return @meth.call(length)
      end
    end

    class Request
      attr_accessor :flag, :length, :message

      def perfect?
        return false if length.nil?
        return false if length.zero?
        return false if message.nil?
        return false if message.empty?
        return true
      end

      def self.urai(flag)
        instance = self.new
        if flag == 0x93
          instance.flag = flag
          instance.length = yield(1)
          instance.message = yield(instance.length)
        end
        return instance
      end
    end

    class Response
      attr_accessor :success, :for_print
    end

    def initialize(remote_meth)
      @remote = RemoteServer.new(remote_meth)
    end

    def make_response(req_raw)
      req = Request.urai(req_raw) { |length| @remote.use(length) }
      resp = Response.new
      resp.success = FLAG_SUCCESS
      resp.for_print = req.message
      return resp
    end
  end
end