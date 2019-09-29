package com.helospark.tactview.openfx;

import com.helospark.lightdi.annotation.ComponentScan;
import com.helospark.lightdi.annotation.Configuration;
import com.helospark.lightdi.annotation.PropertySource;

@Configuration
@ComponentScan
@PropertySource("classpath:openfx-plugin.properties")
public class OpenFXPluginConfiguration {

}
