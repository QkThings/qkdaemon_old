import tornado.ioloop
import tornado.web
import tornado.websocket

import signal

from tornado.ioloop import IOLoop
from tornado.tcpserver import TCPServer

#from tornaduv import UVLoop
#IOLoop.configure(UVLoop)


def handle_signal(sig, frame):
    IOLoop.instance().add_callback(IOLoop.instance().stop)

class EchoWebSocket(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket opened")

    def on_message(self, message):
        self.write_message(u"You said: " + message)

    def on_close(self):
        print("WebSocket closed")

class EchoServer(TCPServer):

    def _handle_data(self, data):
        print "RX:", data
        self._stream.read_bytes(32, self._handle_data, partial=True)

    def handle_stream(self, stream, address):
    	print address, stream
    	self._stream = stream
    	stream.read_bytes(32, self._handle_data, partial=True)
        #self._stream = stream
        #self._read_line()

    def _read_line(self):
        self._stream.read_until('\n', self._handle_read)

    def _handle_read(self, data):
        self._stream.write(data)
        self._read_line()

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Hello, world")

application = tornado.web.Application([
    (r"/", MainHandler),
])

if __name__ == "__main__":
    signal.signal(signal.SIGINT, handle_signal)
    signal.signal(signal.SIGTERM, handle_signal)
    server = EchoServer()
    server.listen(8889)
    IOLoop.current().start()
    #IOLoop.instance().start()
    #IOLoop.instance().close()
    '''
    application.listen(8888)
    IOLoop.current().start()
    '''