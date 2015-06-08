#!/usr/bin/env python3

import logging
logging.basicConfig(level=logging.DEBUG)

from relay import Relay, auto_join, auto_pong
from relay.constants import privmsg

bot = Relay(__name__)

@bot.handler(privmsg)
def echo(target, message, sender, *args, **kwargs):
    if not message.startswith("!echo "):
        return
    sender = sender.split('@')[0].split('!')[0]
    message = message[6:]
    if target == bot.client['nick']:
        yield "PRIVMSG {sender} :{sender}: {message}".format(sender=sender, message=message)
    else:
        yield "PRIVMSG {{target}} :{sender}: {message}".format(sender=sender, message=message)

if __name__ == "__main__":
    bot.register(auto_pong)
    bot.config(from_env=True).run()
