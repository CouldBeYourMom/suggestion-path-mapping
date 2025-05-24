import sqlite3

# Connect to the database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Manually collected video relationships
video_relations = [
       # PARENT    |   SUGGESTED
    ("0CIa07W6GZM", "_XARfBwUhTo"),
    ("0CIa07W6GZM", "L0k1mYA2hq0"),
    ("0CIa07W6GZM", "2Box8dkz6cQ"),
    ("0CIa07W6GZM", "rSpzAVpnXo4"),
    ("0CIa07W6GZM", "din1J2mZmc4"),
    ("0CIa07W6GZM", "bP66ZPXIFP4"),
    ("0CIa07W6GZM", "UHnuoJNjf_c"),
    ("0CIa07W6GZM", "XiUOBdEUqjY"),
    ("0CIa07W6GZM", "h64swlRkcmE"),
    ("0CIa07W6GZM", "JUPrdUvB58c"),

    ("h64swlRkcmE", "GvMz-wtoWrM"),
    ("h64swlRkcmE", "PzugDMRM9Jc"),
    ("h64swlRkcmE", "46XaJvIxPeY"),
    ("h64swlRkcmE", "pzCLQK3YNDc"),
    ("h64swlRkcmE", "1ZXEW_YIftw"),
    ("h64swlRkcmE", "PgpJWPCwLAI"),
    ("h64swlRkcmE", "oI4fSxkqdLU"),
    ("h64swlRkcmE", "063-npQ6pM4"),
    ("h64swlRkcmE", "9wSQ9Zk_9gQ"),
    ("h64swlRkcmE", "gqtrNXdlraM"),

    ("JUPrdUvB58c", "1OSGNzrNXr8"),
    ("JUPrdUvB58c", "88bMVbx1dzM"),
    ("JUPrdUvB58c", "RjS2yIgn3IQ"),
    ("JUPrdUvB58c", "8k25_HmvtyE"),
    ("JUPrdUvB58c", "fYVjknOS_XQ"),
    ("JUPrdUvB58c", "hes_8h2aLRQ"),
    ("JUPrdUvB58c", "PglRkGyXQ1A"),
    ("JUPrdUvB58c", "bFkIG9S2Mmg"),
    ("JUPrdUvB58c", "b_EoZzE7KJ0"),
    ("JUPrdUvB58c", "bshwXGqwA68"),

    ("P-TANCVoHlc", "2ai5NWd8nuo"),
    ("P-TANCVoHlc", "ffw4_-hvZC4"),
    ("P-TANCVoHlc", "0twDETh6QaI"),
    ("P-TANCVoHlc", "b0slsOLp3ng"),
    ("P-TANCVoHlc", "56EyKNjhUDY"),
    ("P-TANCVoHlc", "2f3gB3zSL08"),
    ("P-TANCVoHlc", "kLMZPlIufA0"),
    ("P-TANCVoHlc", "nJbOX4RmjjE"),
    ("P-TANCVoHlc", "k9kmK0St9Jg"),
    ("P-TANCVoHlc", "no_oAmo5hIQ"),

    ("k9kmK0St9Jg", "TLC7JFFPpck"),
    ("k9kmK0St9Jg", "SaiMNBpN9Yw"),
    ("k9kmK0St9Jg", "Xvdk6fmdOtw"),
    ("k9kmK0St9Jg", "YGn5LOb4dk4"),
    ("k9kmK0St9Jg", "5PCqQbLquHA"),
    ("k9kmK0St9Jg", "BPK3TZce-3g"),
    ("k9kmK0St9Jg", "REIlo713ulU"),
    ("k9kmK0St9Jg", "AcKM_e5ig_8"),
    ("k9kmK0St9Jg", "IQJL3htsDyQ"),
    ("k9kmK0St9Jg", "Xo02kRbVDPQ"),

    ("vSynsOf4560", "y08scEk59G0"),
    ("vSynsOf4560", "P-TANCVoHlc"),
    ("vSynsOf4560", "QApHpur4q9A"),
    ("vSynsOf4560", "no_oAmo5hIQ"),
    ("vSynsOf4560", "5z2lPpj3OTQ"),
    ("vSynsOf4560", "_LIYWFt6NEE"),
    ("vSynsOf4560", "Z0jCH7QVhm4"),
    ("vSynsOf4560", "gPqY9WDEplM"),
    ("vSynsOf4560", "iPtS_zEJKKI"),
    ("vSynsOf4560", "DIQIUfCCo1w"),

    ("DyBcSuqLo3k", "Mc4t66r54dU"),
    ("DyBcSuqLo3k", "5z2lPpj3OTQ"),
    ("DyBcSuqLo3k", "VbiYrkR5joM"),
    ("DyBcSuqLo3k", "Pxvfy4qQRog"),
    ("DyBcSuqLo3k", "xt1fiitz1YM"),
    ("DyBcSuqLo3k", "IQJL3htsDyQ"),
    ("DyBcSuqLo3k", "ADYFW7vWvVg"),
    ("DyBcSuqLo3k", "weVVYA_Rsmc"),
    ("DyBcSuqLo3k", "XfDKqucBS0A"),
    ("DyBcSuqLo3k", "tSw04BwQy4M"),

    ("opFKJ1YXPY0", "PVSAAImlVM0"),
    ("opFKJ1YXPY0", "-My82cY8410"),
    ("opFKJ1YXPY0", "KkIkymh5Ayg"),
    ("opFKJ1YXPY0", "GCGwkgUW2sc"),
    ("opFKJ1YXPY0", "5z2lPpj3OTQ"),
    ("opFKJ1YXPY0", "xt1fiitz1YM"),
    ("opFKJ1YXPY0", "bDrNolNVMPg"),
    ("opFKJ1YXPY0", "yIpms96cr1o"),
    ("opFKJ1YXPY0", "IQJL3htsDyQ"),
    ("opFKJ1YXPY0", "k9kmK0St9Jg"),

    ("j74FeqzVuWI", "IJeUsqgwmpg"),
    ("j74FeqzVuWI", "eoFYdhFU5GM"),
    ("j74FeqzVuWI", "0aGNAxN5Z-o"),
    ("j74FeqzVuWI", "UaUfS6Gr_Ik"),
    ("j74FeqzVuWI", "3pvkgUc9Zbc"),
    ("j74FeqzVuWI", "oFwiu8cN3wo"),
    ("j74FeqzVuWI", "NdXMjPPYad4"),
    ("j74FeqzVuWI", "xL3C5K3-RZk"),
    ("j74FeqzVuWI", "QN1rULxGHCA"),
    ("j74FeqzVuWI", "k9kmK0St9Jg"),

    ("PyePMqSmKKs", "G9E6iccJ3JU"),
    ("PyePMqSmKKs", "ab4xh6F3uOo"),
    ("PyePMqSmKKs", "weVVYA_Rsmc"),
    ("PyePMqSmKKs", "9esTFmyWeW0"),
    ("PyePMqSmKKs", "HT7mizKCseQ"),
    ("PyePMqSmKKs", "r6k6OMfsnxg"),
    ("PyePMqSmKKs", "k9kmK0St9Jg"),
    ("PyePMqSmKKs", "PIerWDWwICg"),
    ("PyePMqSmKKs", "Mc4t66r54dU"),
    ("PyePMqSmKKs", "iPtS_zEJKKI"),

    ("uSGVk2KVokQ", "wVYG1mu8Lg8"),
    ("uSGVk2KVokQ", "zyPZuqkLaTc"),
    ("uSGVk2KVokQ", "GhUw-W0Amfs"),
    ("uSGVk2KVokQ", "REIlo713ulU"),
    ("uSGVk2KVokQ", "VLHkxmlj_QU"),
    ("uSGVk2KVokQ", "uEk_pc0xVYc"),
    ("uSGVk2KVokQ", "K297opaBp8E"),
    ("uSGVk2KVokQ", "Li2TQd5nF8s"),
    ("uSGVk2KVokQ", "DJsngculfKo"),
    ("uSGVk2KVokQ", "mmnf2dHPz04"),

    ("Pxvfy4qQRog", "cw0F8G4-dMw"),
    ("Pxvfy4qQRog", "fkZG_FYqOEs"),
    ("Pxvfy4qQRog", "xnLUzZU4UbU"),
    ("Pxvfy4qQRog", "DlMLfxxvKD8"),
    ("Pxvfy4qQRog", "k9kmK0St9Jg"),
    ("Pxvfy4qQRog", "YasnpE7ONNA"),
    ("Pxvfy4qQRog", "EqLfXzzPB0g"),
    ("Pxvfy4qQRog", "vK6fALsenmw"),
    ("Pxvfy4qQRog", "8Q8XoeTCmTQ"),
    ("Pxvfy4qQRog", "IQJL3htsDyQ"),

    ("L4Hxd06UOwU", "2iKigHOe4Jg"),
    ("L4Hxd06UOwU", "8n5Lh_sIN-k"),
    ("L4Hxd06UOwU", "pbmqFEGK73Q"),
    ("L4Hxd06UOwU", "KEgggvchQZE"),
    ("L4Hxd06UOwU", "kejuKpg8U-k"),
    ("L4Hxd06UOwU", "ZDpo_o7dR8c"),
    ("L4Hxd06UOwU", "E2QBRx1XYM0"),
    ("L4Hxd06UOwU", "DXbdGJrydrc"),
    ("L4Hxd06UOwU", "fPQiNDQsx-E"),
    ("L4Hxd06UOwU", "iHXF0-cfqkA"),

    ("WhySWZ68q78", "_uBLsHA8nas"),
    ("WhySWZ68q78", "Pxvfy4qQRog"),
    ("WhySWZ68q78", "k9kmK0St9Jg"),
    ("WhySWZ68q78", "QZBkabrh-N4"),
    ("WhySWZ68q78", "X-XG_FWXSto"),
    ("WhySWZ68q78", "REIlo713ulU"),
    ("WhySWZ68q78", "5z2lPpj3OTQ"),
    ("WhySWZ68q78", "DIQIUfCCo1w"),
    ("WhySWZ68q78", "3isfPdsWaMc"),
    ("WhySWZ68q78", "j0SaNVBJcHU"),

    ("L0k1mYA2hq0", "0a-3U4CfD94"),
    ("L0k1mYA2hq0", "TpZY1d86ZKU"),
    ("L0k1mYA2hq0", "JE5TdcST0LI"),
    ("L0k1mYA2hq0", "0kw9u1vVaNg"),
    ("L0k1mYA2hq0", "twUK5YreLWk"),
    ("L0k1mYA2hq0", "MXAW59fA8hk"),
    ("L0k1mYA2hq0", "V6kMs673lnM"),
    ("L0k1mYA2hq0", "HVZ6-QGylW0"),
    ("L0k1mYA2hq0", "AefTLx9XTGg"),
    ("L0k1mYA2hq0", "qNrnwAKBU3Q"),

    ("-M0FVtHszN4", "UopANFTGexA"),
    ("-M0FVtHszN4", "6a8sARDI4VI"),
    ("-M0FVtHszN4", "Z0jCH7QVhm4"),
    ("-M0FVtHszN4", "HOwMRxIUa1Q"),
    ("-M0FVtHszN4", "wo_e0EvEZn8"),
    ("-M0FVtHszN4", "HHf0DMQ7I9Q"),
    ("-M0FVtHszN4", "gqtrNXdlraM"),
    ("-M0FVtHszN4", "xt1fiitz1YM"),
    ("-M0FVtHszN4", "3aYpG8kn4nc"),
    ("-M0FVtHszN4", "8uYj60gf5SU"),

    ("88bMVbx1dzM", "dFVrncgIvos"),
    ("88bMVbx1dzM", "OreWXw1ojow"),
    ("88bMVbx1dzM", "_IOh0S_L3C4"),
    ("88bMVbx1dzM", "zaXKQ70q4KQ"),
    ("88bMVbx1dzM", "zV2wIfDzFMk"),
    ("88bMVbx1dzM", "8hnu5DspBso"),
    ("88bMVbx1dzM", "2oDXbFcHliM"),
    ("88bMVbx1dzM", "tXLoBs0ipsI"),
    ("88bMVbx1dzM", "Ni82f1-cAXg"),
    ("88bMVbx1dzM", "FnRhnZbDprE"),
    
    ("hes_8h2aLRQ", "CER82OplzWI"),
    ("hes_8h2aLRQ", "RT5jnnttUKw"),
    ("hes_8h2aLRQ", "KokyE0amak0"),
    ("hes_8h2aLRQ", "IUkgHhiezuA"),
    ("hes_8h2aLRQ", "hw2BO4dUYzY"),
    ("hes_8h2aLRQ", "yWbwD6uzqr0"),
    ("hes_8h2aLRQ", "2O3dWrh3USc"),
    ("hes_8h2aLRQ", "a8Jy50f8Ko8"),
    ("hes_8h2aLRQ", "MaykXH37A2k"),
    ("hes_8h2aLRQ", "aGbjZWwyeyA"),

    ("88bMVbx1dzM", "dFVrncgIvos"),
    ("88bMVbx1dzM", "OreWXw1ojow"),
    ("88bMVbx1dzM", "_IOh0S_L3C4"),
    ("88bMVbx1dzM", "zaXKQ70q4KQ"),
    ("88bMVbx1dzM", "zV2wIfDzFMk"),
    ("88bMVbx1dzM", "8hnu5DspBso"),
    ("88bMVbx1dzM", "2oDXbFcHliM"),
    ("88bMVbx1dzM", "tXLoBs0ipsI"),
    ("88bMVbx1dzM", "Ni82f1-cAXg"),
    ("88bMVbx1dzM", "FnRhnZbDprE"),
    
    ("hes_8h2aLRQ", "CER82OplzWI"),
    ("hes_8h2aLRQ", "RT5jnnttUKw"),
    ("hes_8h2aLRQ", "KokyE0amak0"),
    ("hes_8h2aLRQ", "IUkgHhiezuA"),
    ("hes_8h2aLRQ", "hw2BO4dUYzY"),
    ("hes_8h2aLRQ", "yWbwD6uzqr0"),
    ("hes_8h2aLRQ", "2O3dWrh3USc"),
    ("hes_8h2aLRQ", "a8Jy50f8Ko8"),
    ("hes_8h2aLRQ", "MaykXH37A2k"),
    ("hes_8h2aLRQ", "aGbjZWwyeyA"),

]

# Start transaction
conn.execute("BEGIN")

try:
    # Insert data into manual_video_links
    for parent, suggested in video_relations:
        cursor.execute("""
            INSERT OR IGNORE INTO manual_video_links (parent_video, suggested_video)
            VALUES (?, ?)
        """, (parent, suggested))

    # Commit the transaction **only if all updates succeed**
    conn.commit()
    print("✅ Manually added video relationships to database!")

except Exception as e:
    # Rollback if anything goes wrong
    conn.rollback()
    print(f"🚨 Error occurred: {str(e)} — Rolling back changes!")

finally:
    # Close database connection
    conn.close()