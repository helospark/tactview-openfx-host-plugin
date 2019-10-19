package com.helospark.tactview.openfx;

import java.io.File;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.ui.javafx.uicomponents.detailsdata.localizeddetail.LocalizedDetailDomain;
import com.helospark.tactview.ui.javafx.uicomponents.detailsdata.localizeddetail.LocalizedDetailRepositoryChainItem;

@Component
public class OpenfxLocalizedDetailRepositoryChainItem implements LocalizedDetailRepositoryChainItem {
    private OpenFXEffectFactory openFXEffectFactory;

    public OpenfxLocalizedDetailRepositoryChainItem(OpenFXEffectFactory openFXEffectFactory) {
        this.openFXEffectFactory = openFXEffectFactory;
    }

    @Override
    public LocalizedDetailDomain queryData(String id) {
        PluginDescription describeRequest = openFXEffectFactory.getIdToPluginDescription().get(id);
        String pluginId = describeRequest.getPluginId();
        String description = describeRequest.getDescription();
        File contentFolder = describeRequest.getLibraryPath().getParentFile().getParentFile();

        File pluginIcon = new File(contentFolder, "Resources" + File.separator + pluginId + ".png");

        String pluginIconPath = pluginIcon.exists() ? "file:" + pluginIcon.getAbsolutePath() : null;

        LocalizedDetailDomain result = new LocalizedDetailDomain();
        result.setDescription(description);
        result.setIconUrl(pluginIconPath);
        result.setType(id);

        return result;
    }

    @Override
    public boolean supports(String id) {
        return openFXEffectFactory.getIdToPluginDescription().containsKey(id);
    }

}
