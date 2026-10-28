"""Native terminal smoke test in a disposable working directory; never uses player data."""
import os, pty, fcntl, termios, struct, subprocess, tempfile, time, select
from pathlib import Path
root=Path(__file__).resolve().parent.parent
def drain(fd,timeout=.6):
    end=time.time()+timeout;data=b''
    while time.time()<end:
        if select.select([fd],[],[],max(0,end-time.time()))[0]:
            try:data+=os.read(fd,65536)
            except OSError:break
    return data
with tempfile.TemporaryDirectory(prefix='rouge-native-ui-') as cwd:
    m,s=pty.openpty();fcntl.ioctl(s,termios.TIOCSWINSZ,struct.pack('HHHH',50,160,0,0))
    p=subprocess.Popen([root/'build-macos/rouge-dungeon'],stdin=s,stdout=s,stderr=s,cwd=cwd,env={**os.environ,'TERM':'xterm-256color','LANG':'en_US.UTF-8'})
    os.close(s)
    try:
        assert b'ORIGIN' in drain(m, timeout=3), 'Theme selector did not appear within 3 seconds'
        os.write(m,b'\r');drain(m)
        os.write(m,b'\x1bOB'*4+b'\r');assert b'Difficulty' in drain(m)
        os.write(m,b'\x1bOB'*2+b'\r');out=drain(m);assert b'Adventurer' in out and b'ON THE MAP' in out
        assert any(block.encode() in out for block in ('▀','▄','█'))
        os.write(m,b'\x1bOB'*3);assert b'Knight' in drain(m)
        os.write(m,b'h');assert b'FF' in drain(m)
        os.write(m,b'h\r');assert b'Difficulty' in drain(m)
        os.write(m,b'\x1bOB'*2+b'\r');assert b'Knight' in drain(m)
        os.write(m,b'\x1b');assert b'Difficulty' in drain(m)
        print('PASS native minimum-size menus, enlarged glyph preview, selection, highlight and cancel')
    finally:
        p.kill();p.wait(timeout=3);os.close(m)
