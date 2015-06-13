relay
=====


An irc micro-framework that smells too much like a web framework.

Relay is a toy project, its goals were for me to try out framework design,
learn more about the IRC protocol, and to be a replacement for the available
irc python libraries: most of them did not please me.



Installation
------------

I suggest you use a virtualenv

.. code-block:: sh

    $ pip install relay-framework


Example
-------

This is an example of a bot that sends whatever is send after '!echo ' in a PRIVMSG:

.. code-block:: python

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


And to run it:

.. code-block:: sh

    $ RELAY_HOST=irc.example.net RELAY_NICK=echobot python echobot.py


Changelog
---------

:0.1.0:
   Initial version, client accepts handlers, connects and matches data with those.


Todo
----

- Write a decent IRC client implementation
- Write tests for the Relay class
- Write documentation
- Subclass Relay to allow regexp routes
