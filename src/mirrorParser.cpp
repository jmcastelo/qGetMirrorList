#include "mirrorParser.h"

void MirrorParser::setMirrorList(QString data)
{
    rawData = data;    
}

QList<mirror> MirrorParser::getMirrorList()
{
    parseMirrorList();
    return mirrorList;
}

void MirrorParser::parseMirrorList()
{
    mirrorList.clear();

    QStringList lines = rawData.split('\n', QString::SkipEmptyParts);
    
    QString country, mirrorUrl;

    mirror oneMirror;
    oneMirror.url = "";
    oneMirror.country = "";
    oneMirror.protocol = "";
    oneMirror.ipv4 = false;
    oneMirror.ipv6 = false;
    oneMirror.status = false;
    oneMirror.selected = false;
    oneMirror.flag = QPixmap();

    // Skip first 4 lines, they are unwanted comment lines
    for (int i = 4; i < lines.size(); ++i) {
        // Check if country name
        if (lines.at(i).startsWith("##")) {
            country = lines.at(i).section("## ", 1, 1);
            oneMirror.country = country;
            oneMirror.flag = getFlag(country);
        }
        // Check if mirror url
        if (lines.at(i).startsWith("#Server = ")) {
            mirrorUrl = lines.at(i).section("#Server = ", 1, 1);
            oneMirror.url = mirrorUrl;

            // Check protocol
            if (mirrorUrl.startsWith("https")) {
                oneMirror.protocol = "https";
            } else {
                oneMirror.protocol = "http";
            }
            
            mirrorList.append(oneMirror);
        }
    }
}

QPixmap MirrorParser::getFlag(QString country)
{
    QString flag;

    if (country == "Andorra") {
        flag = "ad.png";
    } else if (country == "United Arab Emirates") {
        flag = "ae.png";
    } else if (country == "Afghanistan") {
        flag = "af.png";
    } else if (country == "Antigua and Barbuda") {
        flag = "ag.png";
    } else if (country == "Anguilla") {
        flag = "ai.png";
    } else if (country == "Albania") {
        flag = "al.png";
    } else if (country == "Armenia") {
        flag = "am.png";
    //} else if (country == "") {
        //flag = "an.png";
    } else if (country == "Angola") {
        flag = "ao.png";
    } else if (country == "Argentina") {
        flag = "ar.png";
    } else if (country == "American Samoa") {
        flag = "as.png";
    } else if (country == "Austria") {
        flag = "at.png";
    } else if (country == "Australia") {
        flag = "au.png";
    } else if (country == "Aruba") {
        flag = "aw.png";
    //} else if (country == "") {
        //flag = "ax.png";
    } else if (country == "Azerbaijan") {
        flag = "az.png";
    } else if (country == "Bosnia and Herzegovina") {
        flag = "ba.png";
    } else if (country == "Barbados") {
        flag = "bb.png";
    } else if (country == "Bangladesh") {
        flag = "bd.png";
    } else if (country == "Belgium") {
        flag = "be.png";
    } else if (country == "Burkina Faso") {
        flag = "bf.png";
    } else if (country == "Bulgaria") {
        flag = "bg.png";
    } else if (country == "Bahrain") {
        flag = "bh.png";
    } else if (country == "Burundi") {
        flag = "bi.png";
    } else if (country == "Benin") {
        flag = "bj.png";
    } else if (country == "Bermuda") {
        flag = "bm.png";
    } else if (country == "Brunei") {
        flag = "bn.png";
    } else if (country == "Bolivia") {
        flag = "bo.png";
    } else if (country == "Brazil") {
        flag = "br.png";
    } else if (country == "Bahamas") {
        flag = "bs.png";
    } else if (country == "Bhutan") {
        flag = "bt.png";
    //} else if (country == "") {
        //flag = "bv.png";
    } else if (country == "Botswana") {
        flag = "bw.png";
    } else if (country == "Belarus") {
        flag = "by.png";
    } else if (country == "Belize") {
        flag = "bz.png";
    } else if (country == "Canada") {
        flag = "ca.png";
    //} else if (country == "") {
        //flag = "catalonia.png";
    //} else if (country == "") {
        //flag = "cc.png";
    //} else if (country == "") {
        //flag = "cd.png";
    } else if (country == "Central African Republic") {
        flag = "cf.png";
    } else if (country == "Congo") {
        flag = "cg.png";
    } else if (country == "Switzerland") {
        flag = "ch.png";
    //} else if (country == "") {
        //flag = "ci.png";
    } else if (country == "Cook Islands") {
        flag = "ck.png";
    } else if (country == "Chile") {
        flag = "cl.png";
    } else if (country == "Cameroon") {
        flag = "cm.png";
    } else if (country == "China") {
        flag = "cn.png";
    } else if (country == "Colombia") {
        flag = "co.png";
    } else if (country == "Costa Rica") {
        flag = "cr.png";
    //} else if (country == "") {
        //flag = "cs.png";
    } else if (country == "Cuba") {
        flag = "cu.png";
    } else if (country == "Cabo Verde") {
        flag = "cv.png";
    } else if (country == "Christmas Island") {
        flag = "cx.png";
    } else if (country == "Cyprus") {
        flag = "cy.png";
    } else if (country == "Czechia") {
        flag = "cz.png";
    } else if (country == "Germany") {
        flag = "de.png";
    } else if (country == "Djibouti") {
        flag = "dj.png";
    } else if (country == "Denmark") {
        flag = "dk.png";
    } else if (country == "Dominica") {
        flag = "dm.png";
    } else if (country == "Dominican Republic") {
        flag = "do.png";
    } else if (country == "Algeria") {
        flag = "dz.png";
    } else if (country == "Ecuador") {
        flag = "ec.png";
    } else if (country == "Estonia") {
        flag = "ee.png";
    } else if (country == "Egypt") {
        flag = "eg.png";
    } else if (country == "Wester Sahara") {
        flag = "eh.png";
    //} else if (country == "") {
        //flag = "england.png";
    } else if (country == "Eritrea") {
        flag = "er.png";
    } else if (country == "Spain") {
        flag = "es.png";
    } else if (country == "Ethiopia") {
        flag = "et.png";
    //} else if (country == "") {
        //flag = "europeanunion.png";
    //} else if (country == "") {
        //flag = "fam.png";
    } else if (country == "Finland") {
        flag = "fi.png";
    } else if (country == "Fiji") {
        flag = "fj.png";
    //} else if (country == "") {
        //flag = "fk.png";
    //} else if (country == "") {
        //flag = "fm.png";
    } else if (country == "Faroe Islands") {
        flag = "fo.png";
    } else if (country == "France") {
        flag = "fr.png";
    } else if (country == "Gabon") {
        flag = "ga.png";
    } else if (country == "United Kingdom") {
        flag = "gb.png";
    } else if (country == "Grenada") {
        flag = "gd.png";
    } else if (country == "Georgia") {
        flag = "ge.png";
    } else if (country == "French Guiana") {
        flag = "gf.png";
    } else if (country == "Ghana") {
        flag = "gh.png";
    } else if (country == "Gibraltar") {
        flag = "gi.png";
    } else if (country == "Greenland") {
        flag = "gl.png";
    } else if (country == "Gambia") {
        flag = "gm.png";
    } else if (country == "Guinea") {
        flag = "gn.png";
    } else if (country == "Guadeloupe") {
        flag = "gp.png";
    } else if (country == "Ecuatorial Guinea") {
        flag = "gq.png";
    } else if (country == "Greece") {
        flag = "gr.png";
    //} else if (country == "") {
        //flag = "gs.png";
    } else if (country == "Guatemala") {
        flag = "gt.png";
    } else if (country == "Guam") {
        flag = "gu.png";
    } else if (country == "Guinea-Bissau") {
        flag = "gw.png";
    } else if (country == "Guyana") {
        flag = "gy.png";
    } else if (country == "Hong Kong") {
        flag = "hk.png";
    //} else if (country == "") {
        //flag = "hm.png";
    } else if (country == "Honduras") {
        flag = "hn.png";
    } else if (country == "Croatia") {
        flag = "hr.png";
    } else if (country == "Haiti") {
        flag = "ht.png";
    } else if (country == "Hungary") {
        flag = "hu.png";
    } else if (country == "Indonesia") {
        flag = "id.png";
    } else if (country == "Ireland") {
        flag = "ie.png";
    } else if (country == "Israel") {
        flag = "il.png";
    } else if (country == "India") {
        flag = "in.png";
    //} else if (country == "") {
        //flag = "io.png";
    } else if (country == "Iraq") {
        flag = "iq.png";
    } else if (country == "Iran") {
        flag = "ir.png";
    } else if (country == "Iceland") {
        flag = "is.png";
    } else if (country == "Italy") {
        flag = "it.png";
    } else if (country == "Jamaica") {
        flag = "jm.png";
    } else if (country == "Jordan") {
        flag = "jo.png";
    } else if (country == "Japan") {
        flag = "jp.png";
    } else if (country == "Kenya") {
        flag = "ke.png";
    } else if (country == "Kyrgyzstan") {
        flag = "kg.png";
    } else if (country == "Cambodia") {
        flag = "kh.png";
    } else if (country == "Kiribati") {
        flag = "ki.png";
    } else if (country == "Comoros") {
        flag = "km.png";
    //} else if (country == "") {
        //flag = "kn.png";
    } else if (country == "North Korea") {
        flag = "kp.png";
    } else if (country == "South Korea") {
        flag = "kr.png";
    } else if (country == "Kuwait") {
        flag = "kw.png";
    } else if (country == "Cayman Islands") {
        flag = "ky.png";
    } else if (country == "Kazakhstan") {
        flag = "kz.png";
    } else if (country == "Laos") {
        flag = "la.png";
    } else if (country == "Lebanon") {
        flag = "lb.png";
    } else if (country == "Saint Lucia") {
        flag = "lc.png";
    } else if (country == "Liechtenstein") {
        flag = "li.png";
    } else if (country == "Sri Lanka") {
        flag = "lk.png";
    } else if (country == "Liberia") {
        flag = "lr.png";
    } else if (country == "Lesotho") {
        flag = "ls.png";
    } else if (country == "Lithuania") {
        flag = "lt.png";
    } else if (country == "Luxembourg") {
        flag = "lu.png";
    } else if (country == "Latvia") {
        flag = "lv.png";
    } else if (country == "Libya") {
        flag = "ly.png";
    } else if (country == "Morocco") {
        flag = "ma.png";
    } else if (country == "Monaco") {
        flag = "mc.png";
    } else if (country == "Moldova") {
        flag = "md.png";
    } else if (country == "Montenegro") {
        flag = "me.png";
    } else if (country == "Madagascar") {
        flag = "mg.png";
    } else if (country == "Marshall Islands") {
        flag = "mh.png";
    } else if (country == "North Macedonia") {
        flag = "mk.png";
    } else if (country == "Mali") {
        flag = "ml.png";
    } else if (country == "Myanmar") {
        flag = "mm.png";
    } else if (country == "Mongolia") {
        flag = "mn.png";
    } else if (country == "Macao") {
        flag = "mo.png";
    //} else if (country == "") {
        //flag = "mp.png";
    } else if (country == "Martinique") {
        flag = "mq.png";
    } else if (country == "Mauritania") {
        flag = "mr.png";
    } else if (country == "Montserrat") {
        flag = "ms.png";
    } else if (country == "Malta") {
        flag = "mt.png";
    } else if (country == "Mauritius") {
        flag = "mu.png";
    } else if (country == "Maldives") {
        flag = "mv.png";
    } else if (country == "Malawi") {
        flag = "mw.png";
    } else if (country == "Mexico") {
        flag = "mx.png";
    } else if (country == "Malaysia") {
        flag = "my.png";
    } else if (country == "Mozambique") {
        flag = "mz.png";
    } else if (country == "Namibia") {
        flag = "na.png";
    } else if (country == "New Caledonia") {
        flag = "nc.png";
    } else if (country == "Niger") {
        flag = "ne.png";
    } else if (country == "Norfolk Island") {
        flag = "nf.png";
    } else if (country == "Nigeria") {
        flag = "ng.png";
    } else if (country == "Nicaragua") {
        flag = "ni.png";
    } else if (country == "Netherlands") {
        flag = "nl.png";
    } else if (country == "Norway") {
        flag = "no.png";
    } else if (country == "Nepal") {
        flag = "np.png";
    } else if (country == "Nauru") {
        flag = "nr.png";
    } else if (country == "Niue") {
        flag = "nu.png";
    } else if (country == "New Zealand") {
        flag = "nz.png";
    } else if (country == "Oman") {
        flag = "om.png";
    } else if (country == "Panama") {
        flag = "pa.png";
    } else if (country == "Peru") {
        flag = "pe.png";
    } else if (country == "French Polynesia") {
        flag = "pf.png";
    } else if (country == "Papua New Guinea") {
        flag = "pg.png";
    } else if (country == "Philippines") {
        flag = "ph.png";
    } else if (country == "Pakistan") {
        flag = "pk.png";
    } else if (country == "Poland") {
        flag = "pl.png";
    //} else if (country == "") {
        //flag = "pm.png";
    } else if (country == "Pitcairn") {
        flag = "pn.png";
    } else if (country == "Puerto Rico") {
        flag = "pr.png";
    } else if (country == "Palestine") {
        flag = "ps.png";
    } else if (country == "Portugal") {
        flag = "pt.png";
    } else if (country == "Palau") {
        flag = "pw.png";
    } else if (country == "Paraguay") {
        flag = "py.png";
    } else if (country == "Qatar") {
        flag = "qa.png";
    //} else if (country == "") {
        //flag = "re.png";
    } else if (country == "Romania") {
        flag = "ro.png";
    } else if (country == "Serbia") {
        flag = "rs.png";
    } else if (country == "Russia") {
        flag = "ru.png";
    } else if (country == "Rwanda") {
        flag = "rw.png";
    } else if (country == "Saudi Arabia") {
        flag = "sa.png";
    } else if (country == "Solomon Islands") {
        flag = "sb.png";
    //} else if (country == "") {
        //flag = "scotland.png";
    } else if (country == "Seychelles") {
        flag = "sc.png";
    } else if (country == "Sudan") {
        flag = "sd.png";
    } else if (country == "Sweden") {
        flag = "se.png";
    } else if (country == "Singapore") {
        flag = "sg.png";
    //} else if (country == "") {
        //flag = "sh.png";
    } else if (country == "Slovenia") {
        flag = "si.png";
    //} else if (country == "") {
        //flag = "sj.png";
    } else if (country == "Slovakia") {
        flag = "sk.png";
    } else if (country == "Sierra Leone") {
        flag = "sl.png";
    } else if (country == "San Marino") {
        flag = "sm.png";
    } else if (country == "Senegal") {
        flag = "sn.png";
    } else if (country == "Somalia") {
        flag = "so.png";
    } else if (country == "Suriname") {
        flag = "sr.png";
    //} else if (country == "") {
        //flag = "st.png";
    } else if (country == "El Salvador") {
        flag = "sv.png";
    } else if (country == "Syria") {
        flag = "sy.png";
    } else if (country == "Eswatini") {
        flag = "sz.png";
    //} else if (country == "") {
        //flag = "tc.png";
    } else if (country == "Chad") {
        flag = "td.png";
    //} else if (country == "") {
        //flag = "tf.png";
    } else if (country == "Togo") {
        flag = "tg.png";
    } else if (country == "Thailand") {
        flag = "th.png";
    } else if (country == "Tajikistan") {
        flag = "tj.png";
    } else if (country == "Tokelau") {
        flag = "tk.png";
    //} else if (country == "") {
        //flag = "tl.png";
    } else if (country == "Turkmenistan") {
        flag = "tm.png";
    } else if (country == "Tunisia") {
        flag = "tn.png";
    } else if (country == "Tonga") {
        flag = "to.png";
    } else if (country == "Turkey") {
        flag = "tr.png";
    } else if (country == "Trinidad and Tobago") {
        flag = "tt.png";
    } else if (country == "Tuvalu") {
        flag = "tv.png";
    } else if (country == "Taiwan") {
        flag = "tw.png";
    } else if (country == "Tanzania") {
        flag = "tz.png";
    } else if (country == "Ukraine") {
        flag = "ua.png";
    } else if (country == "Uganda") {
        flag = "ug.png";
    //} else if (country == "") {
        //flag = "um.png";
    } else if (country == "United States") {
        flag = "us.png";
    } else if (country == "Uruguay") {
        flag = "uy.png";
    } else if (country == "Uzbekistan") {
        flag = "uz.png";
    } else if (country == "Holy See") {
        flag = "va.png";
    //} else if (country == "") {
        //flag = "vc.png";
    } else if (country == "Venezuela") {
        flag = "ve.png";
    //} else if (country == "") {
        //flag = "vg.png";
    //} else if (country == "") {
        //flag = "vi.png";
    } else if (country == "Vietnam") {
        flag = "vn.png";
    } else if (country == "Vanuatu") {
        flag = "vu.png";
    //} else if (country == "") {
        //flag = "wales.png";
    } else if (country == "Wallis and Futuna") {
        flag = "wf.png";
    } else if (country == "Samoa") {
        flag = "ws.png";
    } else if (country == "Yemen") {
        flag = "ye.png";
    } else if (country == "Mayotte") {
        flag = "yt.png";
    } else if (country == "South Africa") {
        flag = "za.png";
    } else if (country == "Zambia") {
        flag = "zm.png";
    } else if (country == "Zimbabwe") {
        flag = "zw.png";
    } else if (country == "Worldwide") {
        flag = "world.png";
    }

    QString resource = ":/images/flags/";
    resource += flag;

    return QPixmap(resource);
}

QList<Country> MirrorParser::getCountryList()
{
    parseCountries();
    return countryList;
}

void MirrorParser::parseCountries()
{
    countryList.clear();
    
    Country oneCountry;
    
    QStringList lines = rawData.split('\n', QString::SkipEmptyParts);

    QString countryName;

    // Skip first 4 lines, they are unwanted comment lines
    for (int i = 4; i < lines.size(); ++i) {
        // Check if country name
        if (lines.at(i).startsWith("##")) {
            countryName = lines.at(i).section("## ", 1, 1);
            oneCountry.name = countryName;
            oneCountry.flag = getFlag(countryName);
            countryList.append(oneCountry);
        }
    }
}
