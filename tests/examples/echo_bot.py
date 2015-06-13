#!/usr/bin/env python3

import logging
logging.basicConfig(level=logging.DEBUG)

from relay import Relay, auto_join, auto_pong
from relay.constants import privmsg

bot = Relay("echobot")

@bot.handler(privmsg)
def echo(target, message, sender, *args, **kwargs):
    if not message.startswith("!echo "):
        return
    sender_nick = sender.split('@')[0].split('!')[0] # We just want the nick
    message = message[6:] # We just want whatever is after '!echo '
    if target == bot.client['nick']:
        result = "PRIVMSG {sender_nick} :{sender_nick}: {message}"
    else:
        result = "PRIVMSG {{target}} :{sender_nick}: {message}"
    yield result.format(sender_nick=sender_nick, message=message)

if __name__ == "__main__":
    bot.register(auto_pong)
    bot.register(auto_join(['#tests']))
    bot.config(from_env=True).run()
