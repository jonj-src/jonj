<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:jonj="http://jonj.tk/xsl"
		xsl:extension-element-prefixes="jonj"
                version='1.0'>
<xsl:template match="/">
<xsl:value-of select="jonj:functionString('foobar', /doc/@id, 'secondArg')"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="jonj:function('foobar', /doc/@id)"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="jonj:function('nodeSet')"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="jonj:function('nodeSet',/doc)/i"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="jonj:function('aClass::aStaticFunction','static')"/>
<xsl:text>
</xsl:text>

<xsl:value-of select="jonj:function('nonDomNode')"/>
</xsl:template>
</xsl:stylesheet>
