import sqlite3
from gtts import gTTS
from io import BytesIO
import sys

if __name__ == "__main__":
    lang = sys.argv[1]
    kotoba = sys.argv[2]
    buffer = BytesIO()
    speech = gTTS(text=kotoba, lang=lang, slow=False)
    speech.write_to_fp(buffer)
    buffer.seek(0)
    conn = sqlite3.connect("dic.db")
    cursor = conn.cursor()
    cursor.execute("UPDATE dic SET hatsuon = ? WHERE kotoba = ?", (buffer.read(), kotoba))
    conn.commit()
    conn.close()