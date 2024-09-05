import sqlite3
from gtts import gTTS
import io
import sys

if __name__ == "__main__":
    conn = sqlite3.connect("dic.db")
    cursor = conn.cursor()
    lang = sys.argv[1]
    kotoba = sys.argv[2]
    mp3 = io.BytesIO()
    tts = gTTS(text=kotoba, lang=lang, slow=False)
    tts.write_to_fp(mp3)
    mp3.seek(0)
    cursor.execute("UPDATE dic SET hatsuon = ? WHERE kotoba = ?", (mp3.read(), kotoba))
    conn.commit()
    conn.close()



